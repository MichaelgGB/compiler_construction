// codegen.c
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isprint in comments if needed
#include <math.h>  // For abs()

// --- Global State for Code Generation (managed per function) ---
static FunctionContext *current_function_ctx = NULL;
static StringLiteral *global_string_literals_head = NULL; // For all strings
static int string_literal_counter = 0;
static int unique_label_counter = 0; // For unique labels for comparisons etc.

// --- Helper Function Prototypes ---
static void emit_prologue(FILE *out, const char *function_name, int stack_size);
static void emit_epilogue(FILE *out, const char *function_name);
static void emit_comment(FILE *out, const char *comment);
static const char *operand_to_string(TacOperand op, char *buffer, size_t buffer_size);
static void load_operand_to_register(FILE *out, TacOperand op, const char *reg);
static void store_register_to_operand(FILE *out, const char *reg, TacOperand dest_op);
static const char *get_var_stack_location(const char *var_name);
static void add_local_var_or_temp(const char *name, TypeKind type);
static int get_type_size(TypeKind type);
static void pre_scan_function_locals(TacInstruction *start_instr, TacInstruction *end_instr);
static const char *add_string_literal(const char *str_val);
static void emit_data_section(FILE *out);
static void free_string_literals();
static void free_var_offsets(VarOffset *head);
static void free_function_context();
static char *generate_unique_asm_label(const char *prefix);

// --- Helper Function Implementations ---

char *generate_unique_asm_label(const char *prefix)
{
    char *buf = malloc(32); // Allocate memory for the label
    if (!buf)
    {
        perror("Failed to allocate memory for unique label");
        exit(EXIT_FAILURE);
    }
    sprintf(buf, ".%s%d", prefix, unique_label_counter++);
    return buf; // Caller must free this memory
}

int get_type_size(TypeKind type)
{
    switch (type)
    {
    case TYPE_INT:
    case TYPE_BOOLEAN:   // Booleans stored as 0 or 1 in a word
    case TYPE_INT_ARRAY: // This is a pointer, so 4 bytes on 32-bit
    case TYPE_STRING:    // This is a pointer to char, so 4 bytes
    case TYPE_FLOAT:     // Assuming 32-bit float
        return 4;        // DWORD
    case TYPE_CHAR:
        return 1; // BYTE
    default:
        fprintf(stderr, "Codegen Warning: Unknown type size for %s, defaulting to 4.\n", type_to_string(type));
        return 4;
    }
}

// Adds a variable or temporary to the current function's context if not already present.
// Assigns a stack offset.
static void add_local_var_or_temp(const char *name, TypeKind type)
{
    if (!current_function_ctx || !name)
        return;

    // Check if it already exists
    for (VarOffset *curr = current_function_ctx->locals_head; curr; curr = curr->next)
    {
        if (strcmp(curr->name, name) == 0)
            return;
    }

    VarOffset *new_var = (VarOffset *)malloc(sizeof(VarOffset));
    new_var->name = strdup(name);
    new_var->type = type;
    int size = get_type_size(type);

    // Simple stack alignment to 4-byte boundary
    if (current_function_ctx->current_stack_offset % 4 != 0)
    {
        current_function_ctx->current_stack_offset = (current_function_ctx->current_stack_offset / 4) * 4;
    }
    current_function_ctx->current_stack_offset -= size;
    new_var->offset = current_function_ctx->current_stack_offset;

    new_var->next = current_function_ctx->locals_head;
    current_function_ctx->locals_head = new_var;
    current_function_ctx->total_stack_size = abs(current_function_ctx->current_stack_offset);
}

// Scans TAC for a function to identify all local variables, temporaries, and string literals.
static void pre_scan_function_locals(TacInstruction *start_instr, TacInstruction *end_instr)
{
    if (!current_function_ctx)
        return;
    current_function_ctx->locals_head = NULL;
    current_function_ctx->current_stack_offset = 0;
    current_function_ctx->total_stack_size = 0;

    TacInstruction *current = start_instr;
    while (current)
    {
        if (current->result.type == OP_VAR || current->result.type == OP_TEMP)
        {
            add_local_var_or_temp(current->result.val.name, current->result.data_type);
        }

        // Add string literals to the global list
        if (current->op1.type == OP_STRING_LIT && current->op1.val.name)
        {
            add_string_literal(current->op1.val.name);
        }
        if (current->op2.type == OP_STRING_LIT && current->op2.val.name)
        {
            add_string_literal(current->op2.val.name);
        }
        // Could also be in result, e.g., for some hypothetical TAC
        if (current->result.type == OP_STRING_LIT && current->result.val.name)
        {
            add_string_literal(current->result.val.name);
        }

        if (current == end_instr)
            break;
        current = current->next;
    }
    // Final stack size alignment
    if (current_function_ctx->total_stack_size % 4 != 0)
    {
        current_function_ctx->total_stack_size = (current_function_ctx->total_stack_size / 4 + 1) * 4;
    }
}

static const char *get_var_stack_location(const char *var_name)
{
    static char buffer[40]; // Static buffer, safe for single-threaded use per call

    if (!current_function_ctx || !var_name)
        return "INVALID_VAR_LOCATION";

    for (VarOffset *curr = current_function_ctx->locals_head; curr; curr = curr->next)
    {
        if (strcmp(curr->name, var_name) == 0)
        {
            const char *ptr_type = (curr->type == TYPE_CHAR) ? "BYTE PTR" : "DWORD PTR";
            sprintf(buffer, "%s [ebp%+d]", ptr_type, curr->offset);
            return buffer;
        }
    }

    fprintf(stderr, "Codegen Error: Variable '%s' not found in current function context.\n", var_name);
    sprintf(buffer, "[%s_UNDEFINED]", var_name);
    return buffer;
}

static const char *add_string_literal(const char *str_val)
{
    if (!str_val)
        return NULL;

    for (StringLiteral *curr = global_string_literals_head; curr; curr = curr->next)
    {
        if (strcmp(curr->value, str_val) == 0)
            return curr->label;
    }

    StringLiteral *new_lit = (StringLiteral *)malloc(sizeof(StringLiteral));
    char label_buf[20];
    sprintf(label_buf, "_str%d", string_literal_counter++);
    new_lit->label = strdup(label_buf);
    new_lit->value = strdup(str_val);
    new_lit->next = global_string_literals_head;
    global_string_literals_head = new_lit;
    return new_lit->label;
}

static void emit_prologue(FILE *out, const char *function_name, int stack_size)
{
    fprintf(out, "\n%s:\n", function_name);
    emit_comment(out, "Function prologue");
    fprintf(out, "    push ebp\n");
    fprintf(out, "    mov ebp, esp\n");
    if (stack_size > 0)
    {
        fprintf(out, "    sub esp, %d\n", stack_size);
    }
}

static void emit_epilogue(FILE *out, const char *function_name)
{
    fprintf(out, "\n.epilogue_%s:\n", function_name); // Create a unique label for the epilogue
    emit_comment(out, "Function epilogue");
    fprintf(out, "    mov esp, ebp\n");
    fprintf(out, "    pop ebp\n");
    fprintf(out, "    ret\n");
}

static void emit_comment(FILE *out, const char *comment)
{
    fprintf(out, "    ; %s\n", comment);
}

static const char *operand_to_string(TacOperand op, char *buffer, size_t buffer_size)
{
    switch (op.type)
    {
    case OP_NONE:
        snprintf(buffer, buffer_size, "_");
        break;
    case OP_VAR:
    case OP_TEMP:
    case OP_LABEL:
        snprintf(buffer, buffer_size, "%s", op.val.name ? op.val.name : "NULL_NAME");
        break;
    case OP_STRING_LIT:
        snprintf(buffer, buffer_size, "\"%s\"", op.val.name ? op.val.name : "NULL_STR");
        break;
    case OP_LIT_INT:
        snprintf(buffer, buffer_size, "%d", op.val.int_val);
        break;
    case OP_LIT_FLOAT:
        snprintf(buffer, buffer_size, "%f", op.val.float_val);
        break;
    case OP_LIT_CHAR:
        snprintf(buffer, buffer_size, "'%c'", op.val.char_val);
        break;
    case OP_LIT_BOOL:
        snprintf(buffer, buffer_size, "%s", op.val.int_val ? "true" : "false");
        break;
    default:
        snprintf(buffer, buffer_size, "UNKNOWN_OP");
        break;
    }
    return buffer;
}

static void load_operand_to_register(FILE *out, TacOperand op, const char *reg)
{
    char temp_buf[128];
    switch (op.type)
    {
    case OP_VAR:
    case OP_TEMP:
        if (op.data_type == TYPE_CHAR)
        {
            fprintf(out, "    movzx %s, %s\n", reg, get_var_stack_location(op.val.name));
        }
        else
        {
            fprintf(out, "    mov %s, %s\n", reg, get_var_stack_location(op.val.name));
        }
        break;
    case OP_LIT_INT:
    case OP_LIT_BOOL:
        fprintf(out, "    mov %s, %d\n", reg, op.val.int_val);
        break;
    case OP_LIT_CHAR:
        fprintf(out, "    mov %s, %d\n", reg, (int)op.val.char_val);
        break;
    case OP_STRING_LIT:
        fprintf(out, "    mov %s, %s\n", reg, add_string_literal(op.val.name));
        break;
    default:
        operand_to_string(op, temp_buf, sizeof(temp_buf));
        fprintf(stderr, "Codegen Error: Cannot load operand %s (type %d) to register %s.\n", temp_buf, op.type, reg);
        fprintf(out, "    ; ERROR: Cannot load operand %s to %s\n", temp_buf, reg);
        break;
    }
}

static void store_register_to_operand(FILE *out, const char *reg, TacOperand dest_op)
{
    if (dest_op.type != OP_VAR && dest_op.type != OP_TEMP)
    {
        char temp_buf[128];
        operand_to_string(dest_op, temp_buf, sizeof(temp_buf));
        fprintf(stderr, "Codegen Error: Cannot store register %s to operand %s (type %d).\n", reg, temp_buf, dest_op.type);
        return;
    }
    const char *dest_loc = get_var_stack_location(dest_op.val.name);
    if (dest_op.data_type == TYPE_CHAR)
    {
        const char *byte_reg = NULL;
        if (strcmp(reg, "eax") == 0)
            byte_reg = "al";
        else if (strcmp(reg, "ebx") == 0)
            byte_reg = "bl";
        else if (strcmp(reg, "ecx") == 0)
            byte_reg = "cl";
        else if (strcmp(reg, "edx") == 0)
            byte_reg = "dl";
        if (byte_reg)
        {
            fprintf(out, "    mov %s, %s\n", dest_loc, byte_reg);
        }
        else
        {
            fprintf(out, "    ; ERROR: Cannot get byte-part of register %s for char store\n", reg);
        }
    }
    else
    {
        fprintf(out, "    mov %s, %s\n", dest_loc, reg);
    }
}

static void emit_data_section(FILE *out)
{
    fprintf(out, "\nsection .data\n");
    fprintf(out, "    _int_fmt db \"%%d\", 10, 0\n");
    fprintf(out, "    _char_fmt db \"%%c\", 10, 0\n");
    fprintf(out, "    _true_str db \"true\", 10, 0\n");
    fprintf(out, "    _false_str db \"false\", 10, 0\n");
    fprintf(out, "    _str_nl_fmt db \"%%s\", 10, 0\n"); // Generic format for printing a string with a newline

    for (StringLiteral *curr = global_string_literals_head; curr; curr = curr->next)
    {
        fprintf(out, "    %s db ", curr->label);
        for (int i = 0; curr->value[i] != '\0'; ++i)
        {
            fprintf(out, "0x%02x, ", (unsigned char)curr->value[i]);
        }
        fprintf(out, "0x00 ; \"%s\"\n", curr->value);
    }
}

static void free_string_literals()
{
    StringLiteral *curr = global_string_literals_head;
    while (curr)
    {
        StringLiteral *next = curr->next;
        free(curr->label);
        free(curr->value);
        free(curr);
        curr = next;
    }
    global_string_literals_head = NULL;
    string_literal_counter = 0;
}

static void free_var_offsets(VarOffset *head)
{
    VarOffset *curr = head;
    while (curr)
    {
        VarOffset *next = curr->next;
        free(curr->name);
        free(curr);
        curr = next;
    }
}

static void free_function_context()
{
    if (current_function_ctx)
    {
        free(current_function_ctx->name);
        free_var_offsets(current_function_ctx->locals_head);
        free(current_function_ctx);
        current_function_ctx = NULL;
    }
}

// --- Main Generation Function ---
void generate_x86_assembly(TacInstruction *tac_list, const char *output_filename)
{
    FILE *out = fopen(output_filename, "w");
    if (!out)
    {
        perror("Failed to open output file for assembly");
        return;
    }

    fprintf(out, "bits 32\n");
    fprintf(out, "global main\n");
    fprintf(out, "extern printf\n");
    fprintf(out, "extern exit\n");
    fprintf(out, "extern malloc\n");

    fprintf(out, "\nsection .text\n");

    TacInstruction *current = tac_list;
    char tac_buf_res[128], tac_buf_op1[128], tac_buf_op2[128];

    while (current)
    {
        operand_to_string(current->result, tac_buf_res, sizeof(tac_buf_res));
        operand_to_string(current->op1, tac_buf_op1, sizeof(tac_buf_op1));
        operand_to_string(current->op2, tac_buf_op2, sizeof(tac_buf_op2));

        switch (current->opcode)
        {
        case TAC_FUNCTION_BEGIN:
            fprintf(out, "\n    ; TAC: %s:\n", tac_buf_op1);
            if (current_function_ctx)
            {
                free_function_context();
            } // Should be ended by TAC_FUNCTION_END
            current_function_ctx = (FunctionContext *)calloc(1, sizeof(FunctionContext));
            current_function_ctx->name = strdup(current->op1.val.name);

            TacInstruction *func_end_node = current;
            while (func_end_node && func_end_node->opcode != TAC_FUNCTION_END)
            {
                func_end_node = func_end_node->next;
            }
            pre_scan_function_locals(current, func_end_node);
            emit_prologue(out, current_function_ctx->name, current_function_ctx->total_stack_size);
            break;

        case TAC_FUNCTION_END:
            fprintf(out, "    ; TAC: End %s\n", current_function_ctx ? current_function_ctx->name : "UNKNOWN_FUNCTION");
            if (current_function_ctx)
            {
                emit_epilogue(out, current_function_ctx->name);
                free_function_context();
            }
            break;

        case TAC_ASSIGN:
            fprintf(out, "    ; TAC: %s = %s\n", tac_buf_res, tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_ADD:
        case TAC_SUB:
        case TAC_MUL:
            fprintf(out, "    ; TAC: %s = %s %s %s\n", tac_buf_res, tac_buf_op1,
                    (current->opcode == TAC_ADD ? "+" : (current->opcode == TAC_SUB ? "-" : "*")), tac_buf_op2);
            load_operand_to_register(out, current->op1, "eax");
            load_operand_to_register(out, current->op2, "ebx");
            if (current->opcode == TAC_ADD)
                fprintf(out, "    add eax, ebx\n");
            else if (current->opcode == TAC_SUB)
                fprintf(out, "    sub eax, ebx\n");
            else
                fprintf(out, "    imul eax, ebx\n");
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_DIV:
        case TAC_MOD:
            fprintf(out, "    ; TAC: %s = %s %s %s\n", tac_buf_res, tac_buf_op1, (current->opcode == TAC_DIV ? "/" : "%%"), tac_buf_op2);
            load_operand_to_register(out, current->op1, "eax");
            load_operand_to_register(out, current->op2, "ebx");
            fprintf(out, "    cdq\n");
            fprintf(out, "    idiv ebx\n");
            store_register_to_operand(out, (current->opcode == TAC_DIV) ? "eax" : "edx", current->result);
            break;

        case TAC_UMINUS:
            fprintf(out, "    ; TAC: %s = -%s\n", tac_buf_res, tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    neg eax\n");
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_LABEL:
            fprintf(out, "    ; TAC: %s:\n", tac_buf_res);
            fprintf(out, "%s:\n", current->result.val.name);
            break;

        case TAC_GOTO:
            fprintf(out, "    ; TAC: GOTO %s\n", tac_buf_res);
            fprintf(out, "    jmp %s\n", current->result.val.name);
            break;

        case TAC_LT:
        case TAC_GT:
        case TAC_LEQ:
        case TAC_GEQ:
        case TAC_EQ:
        case TAC_NEQ:
        {
            const char *op_str = "", *jmp_instr = "";
            switch (current->opcode)
            {
            case TAC_LT:
                op_str = "<";
                jmp_instr = "setl";
                break;
            case TAC_GT:
                op_str = ">";
                jmp_instr = "setg";
                break;
            case TAC_LEQ:
                op_str = "<=";
                jmp_instr = "setle";
                break;
            case TAC_GEQ:
                op_str = ">=";
                jmp_instr = "setge";
                break;
            case TAC_EQ:
                op_str = "==";
                jmp_instr = "sete";
                break;
            case TAC_NEQ:
                op_str = "!=";
                jmp_instr = "setne";
                break;
            default:
                break;
            }
            fprintf(out, "    ; TAC: %s = %s %s %s\n", tac_buf_res, tac_buf_op1, op_str, tac_buf_op2);
            load_operand_to_register(out, current->op1, "eax");
            load_operand_to_register(out, current->op2, "ebx");
            fprintf(out, "    cmp eax, ebx\n");
            fprintf(out, "    %s al\n", jmp_instr);
            fprintf(out, "    movzx eax, al\n");
            store_register_to_operand(out, "eax", current->result);
            break;
        }

        case TAC_IF_GOTO:
            fprintf(out, "    ; TAC: IF %s GOTO %s\n", tac_buf_op1, tac_buf_res);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    cmp eax, 0\n");
            fprintf(out, "    jne %s\n", current->result.val.name);
            break;

        case TAC_IF_NOT_GOTO:
            fprintf(out, "    ; TAC: IF NOT %s GOTO %s\n", tac_buf_op1, tac_buf_res);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    cmp eax, 0\n");
            fprintf(out, "    je %s\n", current->result.val.name);
            break;

        case TAC_NOT:
            fprintf(out, "    ; TAC: %s = !%s\n", tac_buf_res, tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    cmp eax, 0\n");
            fprintf(out, "    sete al\n");
            fprintf(out, "    movzx eax, al\n");
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_PRINT:
        {
            fprintf(out, "    ; TAC: PRINT %s\n", tac_buf_op1);
            const char *fmt_str = "_int_fmt";
            int args_to_clean = 8; // default 2 args (format + value)

            if (current->op1.data_type == TYPE_BOOLEAN)
            {
                char *true_label = generate_unique_asm_label("print_true");
                char *end_label = generate_unique_asm_label("print_bool_end");
                load_operand_to_register(out, current->op1, "eax");
                fprintf(out, "    cmp eax, 0\n");
                fprintf(out, "    jne %s\n", true_label);
                fprintf(out, "    push DWORD _false_str\n");
                fprintf(out, "    jmp %s\n", end_label);
                fprintf(out, "%s:\n", true_label);
                fprintf(out, "    push DWORD _true_str\n");
                fprintf(out, "%s:\n", end_label);
                fmt_str = "_str_nl_fmt"; // to print the string ("true" or "false")
                // fall through to call printf
                free(true_label);
                free(end_label);
            }
            else
            {
                load_operand_to_register(out, current->op1, "eax");
                fprintf(out, "    push eax\n");
                if (current->op1.data_type == TYPE_CHAR)
                    fmt_str = "_char_fmt";
                else if (current->op1.data_type == TYPE_STRING)
                    fmt_str = "_str_nl_fmt";
                else
                    fmt_str = "_int_fmt"; // int or other unknown
                fprintf(out, "    push DWORD %s\n", fmt_str);
            }

            fprintf(out, "    call printf\n");
            fprintf(out, "    add esp, %d\n", args_to_clean);
            break;
        }

        case TAC_RETURN:
            fprintf(out, "    ; TAC: RETURN %s\n", tac_buf_op1);
            if (current->op1.type != OP_NONE)
            {
                load_operand_to_register(out, current->op1, "eax");
            }
            if (current_function_ctx && strcmp(current_function_ctx->name, "main") == 0)
            {
                if (current->op1.type == OP_NONE)
                    fprintf(out, "    mov eax, 0\n");
                fprintf(out, "    push eax\n");
                fprintf(out, "    call exit\n");
            }
            else if (current_function_ctx)
            {
                fprintf(out, "    jmp .epilogue_%s\n", current_function_ctx->name);
            }
            break;

        case TAC_PARAM:
            fprintf(out, "    ; TAC: PARAM %s\n", tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    push eax\n");
            break;

        case TAC_CALL:
            fprintf(out, "    ; TAC: %s = CALL %s\n", tac_buf_res, tac_buf_op1);
            fprintf(out, "    call %s\n", current->op1.val.name);
            if (current->op2.type == OP_LIT_INT && current->op2.val.int_val > 0)
            {
                fprintf(out, "    add esp, %d\n", current->op2.val.int_val * 4);
            }
            if (current->result.type != OP_NONE)
            {
                store_register_to_operand(out, "eax", current->result);
            }
            break;

        case TAC_NEW_ARRAY:
        {
            fprintf(out, "    ; TAC: %s = NEW_ARRAY elements: %s\n", tac_buf_res, tac_buf_op1);
            TypeKind element_type = (current->result.data_type == TYPE_INT_ARRAY) ? TYPE_INT : TYPE_INT; // Add other array types here
            int element_size = get_type_size(element_type);
            load_operand_to_register(out, current->op1, "eax");
            if (element_size > 1)
                fprintf(out, "    imul eax, %d\n", element_size);
            fprintf(out, "    push eax\n");
            fprintf(out, "    call malloc\n");
            fprintf(out, "    add esp, 4\n");
            store_register_to_operand(out, "eax", current->result);
            break;
        }

        case TAC_ARRAY_LOAD:
        {
            fprintf(out, "    ; TAC: %s = %s[%s]\n", tac_buf_res, tac_buf_op1, tac_buf_op2);
            TypeKind element_type = (current->op1.data_type == TYPE_INT_ARRAY) ? TYPE_INT : TYPE_INT;
            int element_size = get_type_size(element_type);
            load_operand_to_register(out, current->op1, "ebx"); // base address
            load_operand_to_register(out, current->op2, "ecx"); // index
            if (element_type == TYPE_CHAR)
            {
                fprintf(out, "    movzx eax, BYTE PTR [ebx + ecx*%d]\n", element_size);
            }
            else
            {
                fprintf(out, "    mov eax, DWORD PTR [ebx + ecx*%d]\n", element_size);
            }
            store_register_to_operand(out, "eax", current->result);
            break;
        }

        case TAC_ARRAY_STORE:
        {
            fprintf(out, "    ; TAC: %s[%s] = %s\n", tac_buf_res, tac_buf_op1, tac_buf_op2);
            TypeKind element_type = (current->result.data_type == TYPE_INT_ARRAY) ? TYPE_INT : TYPE_INT;
            int element_size = get_type_size(element_type);
            load_operand_to_register(out, current->result, "ebx"); // base address
            load_operand_to_register(out, current->op1, "ecx");    // index
            load_operand_to_register(out, current->op2, "edx");    // value to store
            if (element_type == TYPE_CHAR)
            {
                fprintf(out, "    mov BYTE PTR [ebx + ecx*%d], dl\n", element_size);
            }
            else
            {
                fprintf(out, "    mov DWORD PTR [ebx + ecx*%d], edx\n", element_size);
            }
            break;
        }

        case TAC_NOP:
            fprintf(out, "    ; TAC: NOP\n    nop\n");
            break;

        default:
            fprintf(out, "    ; ERROR: Unhandled TAC Opcode: %d\n", current->opcode);
            break;
        }
        current = current->next;
    }

    if (current_function_ctx)
    { // Handle case where last function doesn't have an explicit END
        emit_epilogue(out, current_function_ctx->name);
        free_function_context();
    }

    emit_data_section(out);

    fclose(out); // File is now written and closed.
    printf("X86 assembly generated and saved to: %s\n", output_filename);

    // --- Read the generated file and print to terminal ---
    printf("\n--- Generated X86 Assembly (%s) ---\n", output_filename);
    FILE *asm_file_to_read = fopen(output_filename, "r");
    if (asm_file_to_read)
    {
        char line_buffer[1024];
        while (fgets(line_buffer, sizeof(line_buffer), asm_file_to_read) != NULL)
        {
            printf("%s", line_buffer);
        }
        fclose(asm_file_to_read);
    }
    else
    {
        fprintf(stderr, "Warning: Could not re-open %s to print its content to terminal.\n", output_filename);
    }
    printf("---------------------------------------\n");

    // Cleanup global resources
    free_string_literals();
    unique_label_counter = 0;
}