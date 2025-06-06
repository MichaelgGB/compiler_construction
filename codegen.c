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
        // Assuming you have a type_to_string function, if not, you can remove it or implement it
        // fprintf(stderr, "Codegen Warning: Unknown type size for %s, defaulting to 4.\n", type_to_string(type));
        fprintf(stderr, "Codegen Warning: Unknown type size for type %d, defaulting to 4.\n", type);
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
    if (!new_var) { perror("Failed to allocate VarOffset"); exit(EXIT_FAILURE); }
    new_var->name = strdup(name);
    if (!new_var->name) { perror("Failed to strdup name for VarOffset"); free(new_var); exit(EXIT_FAILURE); }
    new_var->type = type;
    int size = get_type_size(type);

    // Simple stack alignment to 4-byte boundary for values larger than a byte, or if offset is not aligned
    if (size > 1 && current_function_ctx->current_stack_offset % size != 0) {
         current_function_ctx->current_stack_offset -= (current_function_ctx->current_stack_offset % size);
    } else if (current_function_ctx->current_stack_offset % 4 != 0 && size == 4) { // Ensure dwords are aligned
         current_function_ctx->current_stack_offset = (current_function_ctx->current_stack_offset / 4) * 4;
    }


    current_function_ctx->current_stack_offset -= size; // Allocate space
    new_var->offset = current_function_ctx->current_stack_offset;

    new_var->next = current_function_ctx->locals_head;
    current_function_ctx->locals_head = new_var;
    // total_stack_size will be calculated at the end of pre_scan_function_locals based on min offset
}

// Scans TAC for a function to identify all local variables, temporaries, and string literals.
static void pre_scan_function_locals(TacInstruction *start_instr, TacInstruction *end_instr)
{
    if (!current_function_ctx)
        return;
    
    // Free existing locals if any (e.g. if this function is called multiple times for different functions)
    free_var_offsets(current_function_ctx->locals_head);
    current_function_ctx->locals_head = NULL;
    current_function_ctx->current_stack_offset = 0; // Start allocating from [ebp-0], going negative
    current_function_ctx->total_stack_size = 0;


    TacInstruction *current = start_instr;
    // First pass: Add all explicitly declared variables (from symbol table if available)
    // For now, we rely on them appearing as results in TAC
    
    // Second pass: Add all temporaries and variables that appear as results
    current = start_instr;
    while (current)
    {
        if (current->result.type == OP_VAR || current->result.type == OP_TEMP)
        {
            // Ensure data_type is set for the result operand during TAC generation
            // if(current->result.data_type == TYPE_UNKNOWN && current->result.val.name) {
            //     // Attempt to infer type, or default. This should ideally be set in TAC phase.
            //     // For simplicity here, we'll assume non-char types are int/dword if not specified.
            //     // A better compiler would have type information from semantic analysis propagated to TAC.
            //     // fprintf(stderr, "Warning: Result operand %s has unknown type, defaulting to TYPE_INT for stack allocation.\n", current->result.val.name);
            //     // current->result.data_type = TYPE_INT; // Defaulting, improve this
            // }
            add_local_var_or_temp(current->result.val.name, current->result.data_type);
        }
         if (current->op1.type == OP_VAR) { // Also consider operands if they are not defined yet (e.g. function params)
            // This part might be more complex depending on how parameters are handled.
            // For simplicity, we assume params are also assigned to via TAC or handled elsewhere.
            // add_local_var_or_temp(current->op1.val.name, current->op1.data_type);
        }
        if (current->op2.type == OP_VAR) {
            // add_local_var_or_temp(current->op2.val.name, current->op2.data_type);
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
        if (current->result.type == OP_STRING_LIT && current->result.val.name)
        {
            add_string_literal(current->result.val.name);
        }

        if (current == end_instr)
            break;
        current = current->next;
    }
    // Calculate total stack size needed based on the lowest offset reached
    current_function_ctx->total_stack_size = abs(current_function_ctx->current_stack_offset);

    // Final stack size alignment to ensure stack pointer (esp) is aligned (typically 4 or 16 byte boundary)
    // For 32-bit, 4-byte alignment is common for general purposes.
    if (current_function_ctx->total_stack_size % 4 != 0)
    {
        current_function_ctx->total_stack_size = (current_function_ctx->total_stack_size / 4 + 1) * 4;
    }
    if (current_function_ctx->total_stack_size == 0 && current_function_ctx->locals_head != NULL) {
        // If there are locals but stack size is 0 (e.g. only one char), ensure some minimal allocation.
        // However, this should be covered by the previous alignment.
        // Or, if only one char, current_stack_offset is -1, abs is 1, aligned to 4.
    }
}

static const char *get_var_stack_location(const char *var_name)
{
    static char buffer[40]; 

    if (!current_function_ctx || !var_name) {
        snprintf(buffer, sizeof(buffer), "INVALID_VAR_NAME");
        return buffer;
    }
    

    for (VarOffset *curr = current_function_ctx->locals_head; curr; curr = curr->next)
    {
        if (strcmp(curr->name, var_name) == 0)
        {
            // NASM syntax does not use "PTR". The size specifier (BYTE, DWORD) is sufficient.
            const char *size_specifier = (curr->type == TYPE_CHAR) ? "BYTE" : "DWORD";
            sprintf(buffer, "%s [ebp%+d]", size_specifier, curr->offset);
            return buffer;
        }
    }

    fprintf(stderr, "Codegen Error: Variable '%s' not found in current function context. Stack location unknown.\n", var_name);
    sprintf(buffer, "[%s_UNDEFINED_IN_CTX]", var_name);
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
    if (!new_lit) { perror("Failed to allocate StringLiteral"); exit(EXIT_FAILURE); }
    char label_buf[20];
    sprintf(label_buf, "_str%d", string_literal_counter++);
    new_lit->label = strdup(label_buf);
    if (!new_lit->label) { perror("Failed to strdup label for StringLiteral"); free(new_lit); exit(EXIT_FAILURE); }
    new_lit->value = strdup(str_val);
    if (!new_lit->value) { perror("Failed to strdup value for StringLiteral"); free(new_lit->label); free(new_lit); exit(EXIT_FAILURE); }
    
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
    // Ensure function_name is not NULL, though it should be set if prologue was emitted
    const char* name_to_use = function_name ? function_name : "unknown_function_epilogue";
    fprintf(out, "\n.epilogue_%s:\n", name_to_use); 
    emit_comment(out, "Function epilogue");
    fprintf(out, "    mov esp, ebp\n");
    fprintf(out, "    pop ebp\n");
    fprintf(out, "    ret\n");
}

static void emit_comment(FILE *out, const char *comment)
{
    if (comment && strlen(comment) > 0) {
        fprintf(out, "    ; %s\n", comment);
    } else {
        fprintf(out, "    ;\n");
    }
}

static const char *operand_to_string(TacOperand op, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0) return "BUF_ERR";
    switch (op.type)
    {
    case OP_NONE:
        snprintf(buffer, buffer_size, "_");
        break;
    case OP_VAR:
    case OP_TEMP:
    case OP_LABEL: // Labels are just names
        snprintf(buffer, buffer_size, "%s", op.val.name ? op.val.name : "NULL_OP_NAME");
        break;
    case OP_STRING_LIT:
        // Ensure op.val.name is not NULL before trying to use it
        snprintf(buffer, buffer_size, "\"%s\"", op.val.name ? op.val.name : "NULL_STR_LIT");
        break;
    case OP_LIT_INT:
        snprintf(buffer, buffer_size, "%d", op.val.int_val);
        break;
    case OP_LIT_FLOAT:
        snprintf(buffer, buffer_size, "%f", op.val.float_val);
        break;
    case OP_LIT_CHAR:
        // Ensure char is printable for comment, otherwise use hex
        if (isprint(op.val.char_val)) {
            snprintf(buffer, buffer_size, "'%c'", op.val.char_val);
        } else {
            snprintf(buffer, buffer_size, "0x%02X", (unsigned char)op.val.char_val);
        }
        break;
    case OP_LIT_BOOL:
        snprintf(buffer, buffer_size, "%s", op.val.int_val ? "true" : "false"); // bool stored as int
        break;
    default:
        snprintf(buffer, buffer_size, "UNKNOWN_OP_TYPE(%d)", op.type);
        break;
    }
    return buffer;
}

static void load_operand_to_register(FILE *out, TacOperand op, const char *reg)
{
    char temp_buf[128];
    const char* op_loc_str;
    switch (op.type)
    {
    case OP_VAR:
    case OP_TEMP:
        op_loc_str = get_var_stack_location(op.val.name);
        if (op.data_type == TYPE_CHAR)
        {
            // For char, ensure we are loading into a full register if needed (e.g. eax, not al directly if using full reg name)
            // movzx (move with zero extend) is good for this.
            fprintf(out, "    movzx %s, %s\n", reg, op_loc_str);
        }
        else
        {
            fprintf(out, "    mov %s, %s\n", reg, op_loc_str);
        }
        break;
    case OP_LIT_INT:
    case OP_LIT_BOOL: // Booleans are 0 or 1, handled as ints
        fprintf(out, "    mov %s, %d\n", reg, op.val.int_val);
        break;
    case OP_LIT_CHAR:
        fprintf(out, "    mov %s, %d\n", reg, (int)op.val.char_val); // Load ASCII value
        break;
    case OP_STRING_LIT: // Load address of string literal
        fprintf(out, "    mov %s, %s\n", reg, add_string_literal(op.val.name));
        break;
    // Add OP_LABEL if you intend to load addresses of labels, though usually labels are targets for jumps.
    // case OP_LABEL:
    //    fprintf(out, "    mov %s, %s\n", reg, op.val.name);
    //    break;
    default:
        operand_to_string(op, temp_buf, sizeof(temp_buf));
        fprintf(stderr, "Codegen Error: Cannot load operand %s (type %d, value %s) to register %s.\n", 
                temp_buf, op.type, op.val.name ? op.val.name : "N/A", reg);
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
        fprintf(out, "    ; ERROR: Cannot store register %s to %s (invalid dest type)\n", reg, temp_buf);
        return;
    }
    
    const char *dest_loc = get_var_stack_location(dest_op.val.name);
    if (dest_op.data_type == TYPE_CHAR)
    {
        const char *byte_reg = NULL;
        // Ensure 'reg' is one of the general purpose registers that have an 8-bit part
        if (strcmp(reg, "eax") == 0) byte_reg = "al";
        else if (strcmp(reg, "ebx") == 0) byte_reg = "bl";
        else if (strcmp(reg, "ecx") == 0) byte_reg = "cl";
        else if (strcmp(reg, "edx") == 0) byte_reg = "dl";
        // Add esi, edi if you use their byte parts (sil, dil) - less common for this
        
        if (byte_reg)
        {
            fprintf(out, "    mov %s, %s\n", dest_loc, byte_reg);
        }
        else
        {
            // This case implies 'reg' was not eax, ebx, ecx, or edx, or was already a byte reg string.
            // If 'reg' could be "al", "bl", etc. directly, handle that. For simplicity, assume full reg name.
            fprintf(stderr, "Codegen Error: Cannot get byte-part of register %s for char store to %s.\n", reg, dest_op.val.name);
            fprintf(out, "    ; ERROR: Cannot get byte-part of register %s for char store\n", reg);
        }
    }
    else // For DWORD types (int, bool, pointers)
    {
        fprintf(out, "    mov %s, %s\n", dest_loc, reg);
    }
}

static void emit_data_section(FILE *out)
{
    fprintf(out, "\nsection .data\n");
    // Standard format strings
    fprintf(out, "    _int_fmt db \"%%d\", 10, 0\n");       // For integers, with newline
    fprintf(out, "    _char_fmt db \"%%c\", 10, 0\n");      // For chars, with newline
    fprintf(out, "    _true_str db \"true\", 10, 0\n");    // For boolean true, with newline
    fprintf(out, "    _false_str db \"false\", 10, 0\n");  // For boolean false, with newline
    fprintf(out, "    _str_nl_fmt db \"%%s\", 10, 0\n");  // For general strings from variables, with newline
    // Add other necessary global data here, e.g. for floating point if supported

    // User-defined string literals
    StringLiteral *curr = global_string_literals_head;
    if (curr == NULL && string_literal_counter > 0) {
        // This might indicate an issue if add_string_literal was called but head is null
    }
    while (curr)
    {
        fprintf(out, "    %s db ", curr->label);
        // Print string as comma-separated hex bytes, then null terminator
        for (int i = 0; curr->value[i] != '\0'; ++i)
        {
            fprintf(out, "0x%02x, ", (unsigned char)curr->value[i]);
        }
        fprintf(out, "0x00 ; \"");
        // Print original string in comment, escaping special chars for the comment
        for (int i = 0; curr->value[i] != '\0'; ++i) {
            char c = curr->value[i];
            if (c == '"') fprintf(out, "\\\"");
            else if (c == '\\') fprintf(out, "\\\\");
            else if (isprint(c)) fprintf(out, "%c", c);
            else fprintf(out, "\\x%02x", (unsigned char)c);
        }
        fprintf(out, "\"\n");
        curr = curr->next;
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
    string_literal_counter = 0; // Reset for potential multiple compilations in one run (if tool supports it)
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
        free(current_function_ctx->name); // strdup'd
        free_var_offsets(current_function_ctx->locals_head);
        current_function_ctx->locals_head = NULL; // Avoid double free if called again
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

    // Reset global state for this generation pass
    free_string_literals(); // Clear any previous literals
    unique_label_counter = 0; // Reset unique label counter for this assembly file


    fprintf(out, "bits 32\n");
    fprintf(out, "global main         ; Make main visible to linker\n");
    fprintf(out, "extern printf       ; External C library functions\n");
    fprintf(out, "extern exit\n");
    fprintf(out, "extern malloc       ; For 'new' operator if you have one\n");
    // Add other externs if needed (e.g., for floating point, string functions from C)

    fprintf(out, "\nsection .text\n");

    TacInstruction *current = tac_list;
    char tac_buf_res[128], tac_buf_op1[128], tac_buf_op2[128];

    while (current)
    {
        // Generate string representations of TAC operands for comments
        operand_to_string(current->result, tac_buf_res, sizeof(tac_buf_res));
        operand_to_string(current->op1, tac_buf_op1, sizeof(tac_buf_op1));
        operand_to_string(current->op2, tac_buf_op2, sizeof(tac_buf_op2));

        // Emit TAC as comment
        // You might want a more structured way to print the TAC instruction itself as a comment
        // For now, a simple switch or if/else for formatting the comment:
        // Example: fprintf(out, "    ; TAC: %s = %s op %s\n", tac_buf_res, tac_buf_op1, tac_buf_op2);

        switch (current->opcode)
        {
        case TAC_FUNCTION_BEGIN:
            fprintf(out, "\n    ; TAC: FUNCTION_BEGIN %s\n", tac_buf_op1); // op1 is function name
            if (current_function_ctx) { // Should not happen if TAC_FUNCTION_END was processed
                fprintf(stderr, "Warning: Overwriting active function context for %s with %s\n", 
                        current_function_ctx->name, current->op1.val.name);
                free_function_context();
            }
            current_function_ctx = (FunctionContext *)calloc(1, sizeof(FunctionContext));
            if (!current_function_ctx) { perror("Failed to allocate FunctionContext"); exit(EXIT_FAILURE); }
            current_function_ctx->name = strdup(current->op1.val.name);
            if (!current_function_ctx->name) { perror("Failed to strdup function name"); free(current_function_ctx); current_function_ctx = NULL; exit(EXIT_FAILURE); }


            // Find the end of the function to scan all its locals and temporaries
            TacInstruction *func_end_node = current->next; // Start scan from instruction after BEGIN
            while (func_end_node && func_end_node->opcode != TAC_FUNCTION_END)
            {
                // Also check that we don't run into another FUNCTION_BEGIN
                if (func_end_node->opcode == TAC_FUNCTION_BEGIN) {
                    fprintf(stderr, "Error: Nested TAC_FUNCTION_BEGIN without TAC_FUNCTION_END for %s.\n", current_function_ctx->name);
                    // Handle error: maybe stop, or just scan up to here
                    break; 
                }
                func_end_node = func_end_node->next;
            }
            // If func_end_node is NULL here, it means TAC_FUNCTION_END was not found for this function.
            // pre_scan_function_locals expects the end_instr to be the TAC_FUNCTION_END itself or NULL if it's the last func.
            // If func_end_node is NULL, it implies scanning till the end of TAC list.
            // The pre_scan_function_locals was modified to handle 'current' up to 'end_instr' (inclusive of start, exclusive of end for scan range logic)
            // Let's adjust: pre_scan from current (BEGIN) up to func_end_node (END)
            pre_scan_function_locals(current, func_end_node); 
            emit_prologue(out, current_function_ctx->name, current_function_ctx->total_stack_size);
            break;

        case TAC_FUNCTION_END:
            fprintf(out, "    ; TAC: FUNCTION_END %s\n", current_function_ctx ? current_function_ctx->name : "UNKNOWN_FUNCTION");
            if (current_function_ctx)
            {
                // Make sure the name matches if op1 is used for function name in TAC_FUNCTION_END
                // if (current->op1.type == OP_LABEL && strcmp(current_function_ctx->name, current->op1.val.name) != 0) {
                //    fprintf(stderr, "Warning: TAC_FUNCTION_END for %s does not match current context %s\n", current->op1.val.name, current_function_ctx->name);
                // }
                emit_epilogue(out, current_function_ctx->name);
                free_function_context(); // Free context for this function
            } else {
                 fprintf(stderr, "Warning: TAC_FUNCTION_END encountered without active function context.\n");
                 // emit_epilogue(out, "orphaned_function_end"); // Or some default
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
            // For comments
            {
                const char *op_symbol = "";
                if(current->opcode == TAC_ADD) op_symbol = "+";
                else if(current->opcode == TAC_SUB) op_symbol = "-";
                else if(current->opcode == TAC_MUL) op_symbol = "*";
                fprintf(out, "    ; TAC: %s = %s %s %s\n", tac_buf_res, tac_buf_op1, op_symbol, tac_buf_op2);
            }
            load_operand_to_register(out, current->op1, "eax");
            load_operand_to_register(out, current->op2, "ebx");
            if (current->opcode == TAC_ADD)
                fprintf(out, "    add eax, ebx\n");
            else if (current->opcode == TAC_SUB)
                fprintf(out, "    sub eax, ebx\n");
            else // TAC_MUL
                fprintf(out, "    imul eax, ebx\n"); // Signed multiply
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_DIV:
        case TAC_MOD:
             {
                const char *op_symbol = (current->opcode == TAC_DIV) ? "/" : "%%";
                fprintf(out, "    ; TAC: %s = %s %s %s\n", tac_buf_res, tac_buf_op1, op_symbol, tac_buf_op2);
            }
            load_operand_to_register(out, current->op1, "eax"); // Dividend
            load_operand_to_register(out, current->op2, "ebx"); // Divisor
            fprintf(out, "    cdq\n");                     // Sign-extend EAX into EDX:EAX
            fprintf(out, "    idiv ebx\n");                  // Quotient in EAX, Remainder in EDX
            store_register_to_operand(out, (current->opcode == TAC_DIV) ? "eax" : "edx", current->result);
            break;

        case TAC_UMINUS: // Unary minus
            fprintf(out, "    ; TAC: %s = -%s\n", tac_buf_res, tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    neg eax\n");
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_LABEL:
            // result operand holds the label name
            fprintf(out, "    ; TAC: LABEL %s:\n", tac_buf_res);
            fprintf(out, "%s:\n", current->result.val.name);
            break;

        case TAC_GOTO:
            // result operand holds the target label name
            fprintf(out, "    ; TAC: GOTO %s\n", tac_buf_res);
            fprintf(out, "    jmp %s\n", current->result.val.name);
            break;
        
        // Comparison operators: result = op1 CMP_OP op2
        case TAC_LT: case TAC_GT: case TAC_LEQ: case TAC_GEQ: case TAC_EQ: case TAC_NEQ:
        {
            const char *op_str = "?"; const char *set_instr = "INVALID_SET_INSTR";
            switch (current->opcode) {
                case TAC_LT:  op_str = "<";  set_instr = "setl";  break; // Set if Less
                case TAC_GT:  op_str = ">";  set_instr = "setg";  break; // Set if Greater
                case TAC_LEQ: op_str = "<="; set_instr = "setle"; break; // Set if Less or Equal
                case TAC_GEQ: op_str = ">="; set_instr = "setge"; break; // Set if Greater or Equal
                case TAC_EQ:  op_str = "=="; set_instr = "sete";  break; // Set if Equal
                case TAC_NEQ: op_str = "!="; set_instr = "setne"; break; // Set if Not Equal
                default: break; // Should not happen
            }
            fprintf(out, "    ; TAC: %s = %s %s %s\n", tac_buf_res, tac_buf_op1, op_str, tac_buf_op2);
            load_operand_to_register(out, current->op1, "eax");
            load_operand_to_register(out, current->op2, "ebx");
            fprintf(out, "    cmp eax, ebx\n");
            fprintf(out, "    %s al\n", set_instr);      // Set AL (8-bit) based on comparison
            fprintf(out, "    movzx eax, al\n");         // Zero-extend AL to EAX (result is 0 or 1)
            store_register_to_operand(out, "eax", current->result);
            break;
        }

        case TAC_IF_GOTO: // IF op1 GOTO result(label)
            fprintf(out, "    ; TAC: IF %s GOTO %s\n", tac_buf_op1, tac_buf_res);
            load_operand_to_register(out, current->op1, "eax"); // Load boolean condition
            fprintf(out, "    cmp eax, 0\n");           // Compare with false (0)
            fprintf(out, "    jne %s\n", current->result.val.name); // Jump if Not Equal (i.e., if true)
            break;

        case TAC_IF_NOT_GOTO: // IF NOT op1 GOTO result(label)
            fprintf(out, "    ; TAC: IF_NOT %s GOTO %s\n", tac_buf_op1, tac_buf_res);
            load_operand_to_register(out, current->op1, "eax"); // Load boolean condition
            fprintf(out, "    cmp eax, 0\n");            // Compare with false (0)
            fprintf(out, "    je %s\n", current->result.val.name);  // Jump if Equal (i.e., if false)
            break;

        case TAC_NOT: // result = !op1
            fprintf(out, "    ; TAC: %s = NOT %s\n", tac_buf_res, tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    cmp eax, 0\n");   // Check if eax is 0 (false)
            fprintf(out, "    sete al\n");      // Set AL to 1 if eax was 0, else 0
            fprintf(out, "    movzx eax, al\n"); // Zero-extend AL to EAX
            store_register_to_operand(out, "eax", current->result);
            break;

        case TAC_PRINT:
        {
            fprintf(out, "    ; TAC: PRINT %s\n", tac_buf_op1);
            const char *fmt_str_label = NULL;
            int args_to_clean = 8; // Default: format string + 1 value = 2 * 4 bytes

            if (current->op1.data_type == TYPE_BOOLEAN) {
                // For boolean, print "true" or "false" string
                char *true_branch_label = generate_unique_asm_label("print_true");
                char *end_print_bool_label = generate_unique_asm_label("print_bool_end");
                
                load_operand_to_register(out, current->op1, "eax");
                fprintf(out, "    cmp eax, 0\n");
                fprintf(out, "    jne %s\n", true_branch_label); // If not 0 (true), jump to print "true"
                // False case:
                fprintf(out, "    push DWORD _false_str\n");
                fprintf(out, "    jmp %s\n", end_print_bool_label);
                // True case:
                fprintf(out, "%s:\n", true_branch_label);
                fprintf(out, "    push DWORD _true_str\n");
                // End of conditional push:
                fprintf(out, "%s:\n", end_print_bool_label);
                
                fmt_str_label = "_str_nl_fmt"; // Use string format for "true"/"false"
                // free(true_branch_label); // Freed after use
                // free(end_print_bool_label);
            } else if (current->op1.data_type == TYPE_STRING) {
                // If operand is already a string (e.g. string variable/literal)
                load_operand_to_register(out, current->op1, "eax"); // Loads address of string
                fprintf(out, "    push eax\n");
                fmt_str_label = "_str_nl_fmt";
            } else { // Int, Char
                load_operand_to_register(out, current->op1, "eax");
                fprintf(out, "    push eax\n"); // Push the value
                if (current->op1.data_type == TYPE_CHAR) {
                    fmt_str_label = "_char_fmt";
                } else { // TYPE_INT or unknown defaults to int
                    fmt_str_label = "_int_fmt";
                }
            }
            
            fprintf(out, "    push DWORD %s\n", fmt_str_label); // Push format string address
            fprintf(out, "    call printf\n");
            fprintf(out, "    add esp, %d\n", args_to_clean); // Clean up stack (2 DWORDs pushed)
            break;
        }


        case TAC_RETURN: // RETURN op1 (op1 can be OP_NONE)
            fprintf(out, "    ; TAC: RETURN %s\n", tac_buf_op1);
            if (current->op1.type != OP_NONE) {
                load_operand_to_register(out, current->op1, "eax"); // Return value in EAX
            }

            // Special handling for return from 'main' -> call exit
            if (current_function_ctx && strcmp(current_function_ctx->name, "main") == 0) {
                if (current->op1.type == OP_NONE) { // Implicit return 0 from main if no value specified
                    fprintf(out, "    mov eax, 0\n");
                }
                // For main, we typically call exit with the return code
                fprintf(out, "    push eax\n");    // Push return code as argument to exit
                fprintf(out, "    call exit\n");   // exit() does not return
                // No jmp to epilogue needed here as exit terminates.
            } else if (current_function_ctx) { // For other functions, jump to epilogue
                fprintf(out, "    jmp .epilogue_%s\n", current_function_ctx->name);
            } else {
                 fprintf(out, "    ; Warning: RETURN in unknown function context, emitting direct ret\n");
                 fprintf(out, "    ret\n");
            }
            break;

        case TAC_PARAM: // PARAM op1
            fprintf(out, "    ; TAC: PARAM %s\n", tac_buf_op1);
            load_operand_to_register(out, current->op1, "eax");
            fprintf(out, "    push eax\n"); // Push parameter onto stack
            break;

        case TAC_CALL: // result = CALL op1 (function_name), op2 (num_args_as_lit_int)
            fprintf(out, "    ; TAC: %s = CALL %s, (args: %s)\n", tac_buf_res, tac_buf_op1, tac_buf_op2);
            fprintf(out, "    call %s\n", current->op1.val.name); // op1.val.name is function label
            
            // Clean up parameters from stack if op2 specifies num_args
            if (current->op2.type == OP_LIT_INT && current->op2.val.int_val > 0) {
                fprintf(out, "    add esp, %d\n", current->op2.val.int_val * 4); // Each param is 4 bytes
            }
            
            if (current->result.type != OP_NONE) { // If call has a return value to store
                store_register_to_operand(out, "eax", current->result); // Return value in EAX
            }
            break;
        
        case TAC_NEW_ARRAY: // result = NEW_ARRAY op1 (size in elements)
        {
            // Assuming op1 is number of elements, result.data_type is e.g. TYPE_INT_ARRAY
            fprintf(out, "    ; TAC: %s = NEW_ARRAY elements: %s\n", tac_buf_res, tac_buf_op1);
            // Determine element size based on result's array type
            TypeKind element_type_kind = TYPE_INT; // Default
            if (current->result.data_type == TYPE_INT_ARRAY) element_type_kind = TYPE_INT;
            // else if (current->result.data_type == TYPE_CHAR_ARRAY) element_type_kind = TYPE_CHAR; // etc.
            
            int element_size_bytes = get_type_size(element_type_kind);
            
            load_operand_to_register(out, current->op1, "eax"); // Number of elements in eax
            if (element_size_bytes > 1) { // Multiply by element size to get total bytes
                fprintf(out, "    imul eax, %d\n", element_size_bytes);
            }
            fprintf(out, "    push eax\n");    // Push size in bytes as argument to malloc
            fprintf(out, "    call malloc\n");
            fprintf(out, "    add esp, 4\n");    // Clean up argument from stack
            store_register_to_operand(out, "eax", current->result); // Store allocated pointer in result
            break;
        }

        case TAC_ARRAY_LOAD: // result = op1[op2]  (op1=base_addr, op2=index)
        {
            fprintf(out, "    ; TAC: %s = %s[%s]\n", tac_buf_res, tac_buf_op1, tac_buf_op2);
            // Determine element type from op1 (the array variable)
            TypeKind element_type_kind = TYPE_INT; // Default, should be derived from op1.data_type
            if (current->op1.data_type == TYPE_INT_ARRAY) element_type_kind = TYPE_INT;
            // else if (current->op1.data_type == TYPE_CHAR_ARRAY) element_type_kind = TYPE_CHAR;

            int element_size_bytes = get_type_size(element_type_kind);

            load_operand_to_register(out, current->op1, "ebx"); // Base address of array into EBX
            load_operand_to_register(out, current->op2, "ecx"); // Index into ECX
            
            // Address calculation: ebx + ecx * element_size_bytes
            if (element_type_kind == TYPE_CHAR) {
                // For movzx, the source (memory) is byte, destination (eax) is dword
                fprintf(out, "    movzx eax, BYTE [ebx + ecx*%d]\n", element_size_bytes);
            } else { // DWORD elements
                fprintf(out, "    mov eax, DWORD [ebx + ecx*%d]\n", element_size_bytes);
            }
            store_register_to_operand(out, "eax", current->result);
            break;
        }

        case TAC_ARRAY_STORE: // result[op1] = op2 (result=base_addr, op1=index, op2=value)
        {
            fprintf(out, "    ; TAC: %s[%s] = %s\n", tac_buf_res, tac_buf_op1, tac_buf_op2);
            TypeKind element_type_kind = TYPE_INT; // Default, should be derived from result.data_type
            if (current->result.data_type == TYPE_INT_ARRAY) element_type_kind = TYPE_INT;
            // else if (current->result.data_type == TYPE_CHAR_ARRAY) element_type_kind = TYPE_CHAR;

            int element_size_bytes = get_type_size(element_type_kind);

            load_operand_to_register(out, current->result, "ebx"); // Base address into EBX
            load_operand_to_register(out, current->op1, "ecx");    // Index into ECX
            load_operand_to_register(out, current->op2, "edx");    // Value to store into EDX
            
            if (element_type_kind == TYPE_CHAR) {
                // Storing a byte (dl) into memory
                fprintf(out, "    mov BYTE [ebx + ecx*%d], dl\n", element_size_bytes);
            } else { // DWORD elements
                fprintf(out, "    mov DWORD [ebx + ecx*%d], edx\n", element_size_bytes);
            }
            break;
        }
        
        case TAC_NOP:
            fprintf(out, "    ; TAC: NOP\n");
            fprintf(out, "    nop\n");
            break;

        default:
            fprintf(out, "    ; ERROR: Unhandled TAC Opcode: %d (%s = %s op %s)\n", 
                current->opcode, tac_buf_res, tac_buf_op1, tac_buf_op2);
            // You might want to exit or throw an error for unhandled opcodes
            break;
        }
        current = current->next;
    }

    // If the last function didn't have an explicit TAC_FUNCTION_END,
    // but there was an active context, emit its epilogue.
    if (current_function_ctx)
    {
        fprintf(stderr, "Warning: Active function context for %s at end of TAC list without TAC_FUNCTION_END.\n", current_function_ctx->name);
        emit_epilogue(out, current_function_ctx->name);
        free_function_context();
    }

    emit_data_section(out);

    fclose(out);
    printf("X86 assembly generated and saved to: %s\n", output_filename);

    // --- Read the generated file and print to terminal ---
    // This part is for convenience during development/debugging
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

    // Cleanup global resources that persist across calls to generate_x86_assembly (if any beyond string literals)
    // String literals are reset at the start of the function for each call now.
    // unique_label_counter is also reset.
}