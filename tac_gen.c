#include "tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global pointers for the TAC instruction list
TacInstruction *tac_head = NULL;
TacInstruction *tac_tail = NULL;

// Counters for unique temporary variables and labels
static int temp_counter = 0;
static int label_counter = 0;

void init_tac_generator()
{
    tac_head = NULL;
    tac_tail = NULL;
    temp_counter = 0;
    label_counter = 0;
}

const char* type_to_string(TypeKind type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_BOOLEAN: return "bool";
        case TYPE_VOID: return "void";
        case TYPE_INT_ARRAY: return "int[]";
        case TYPE_FLOAT: return "float"; // If you added this
        case TYPE_CHAR: return "char";   // If you added this
        case TYPE_STRING: return "string"; // If you added this
        case TYPE_ERROR: return "error";
        default: return "unknown_type";
    }
}

TacInstruction *create_tac_instruction(TacOpCode opcode, TacOperand result, TacOperand op1, TacOperand op2, int line_num)
{
    TacInstruction *instr = (TacInstruction *)malloc(sizeof(TacInstruction));
    if (!instr)
    {
        fprintf(stderr, "Fatal: Memory allocation failed for TAC instruction.\n");
        exit(1);
    }
    instr->opcode = opcode;
    instr->result = result;
    instr->op1 = op1;
    instr->op2 = op2;
    instr->next = NULL;
    instr->line_number = line_num;
    return instr;
}

void emit_tac(TacOpCode opcode, TacOperand result, TacOperand op1, TacOperand op2, int line_num)
{
    TacInstruction *new_instr = create_tac_instruction(opcode, result, op1, op2, line_num);
    if (tac_head == NULL)
    {
        tac_head = new_instr;
        tac_tail = new_instr;
    }
    else
    {
        tac_tail->next = new_instr;
        tac_tail = new_instr;
    }
}

// --- Operand Creation Helpers ---

TacOperand create_tac_none_operand()
{
    TacOperand op;
    op.type = OP_NONE;
    op.data_type = TYPE_VOID; // or TYPE_UNKNOWN
    return op;
}

TacOperand create_tac_var_operand(const char *name, TypeKind type)
{
    TacOperand op;
    op.type = OP_VAR;
    op.val.name = strdup(name); // Duplicate name as it might be temporary
    op.data_type = type;
    return op;
}

TacOperand create_tac_temp_operand(TypeKind type)
{
    TacOperand op;
    op.type = OP_TEMP;
    op.val.name = generate_new_temp_name();
    op.data_type = type;
    return op;
}

TacOperand create_tac_int_literal_operand(int val)
{
    TacOperand op;
    op.type = OP_LIT_INT;
    op.val.int_val = val;
    op.data_type = TYPE_INT;
    return op;
}

TacOperand create_tac_float_literal_operand(float val)
{
    TacOperand op;
    op.type = OP_LIT_FLOAT;
    op.val.float_val = val;
    op.data_type = TYPE_FLOAT; // Assuming you add float type support
    return op;
}

TacOperand create_tac_char_literal_operand(char val)
{
    TacOperand op;
    op.type = OP_LIT_CHAR;
    op.val.char_val = val;
    op.data_type = TYPE_CHAR;
    return op;
}

TacOperand create_tac_bool_literal_operand(int val)
{
    TacOperand op;
    op.type = OP_LIT_BOOL;
    op.val.int_val = val; // 0 for false, 1 for true
    op.data_type = TYPE_BOOLEAN;
    return op;
}

TacOperand create_tac_string_literal_operand(const char *val)
{
    TacOperand op;
    op.type = OP_STRING_LIT;
    op.val.name = strdup(val);
    op.data_type = TYPE_STRING; // Assuming you add string type support
    return op;
}

TacOperand create_tac_label_operand(const char *label_name)
{
    TacOperand op;
    op.type = OP_LABEL;
    op.val.name = strdup(label_name);
    op.data_type = TYPE_VOID; // Labels don't have a data type in this context
    return op;
}

// --- Name Generation Helpers ---

char *generate_new_temp_name()
{
    char *name = (char *)malloc(32); // Sufficiently large
    if (!name)
    {
        fprintf(stderr, "Fatal: Memory allocation failed for temp name.\n");
        exit(1);
    }
    sprintf(name, "_t%d", temp_counter++);
    return name;
}

char *generate_new_label_name()
{
    char *name = (char *)malloc(32); // Sufficiently large
    if (!name)
    {
        fprintf(stderr, "Fatal: Memory allocation failed for label name.\n");
        exit(1);
    }
    sprintf(name, "L%d", label_counter++);
    return name;
}

// --- Debugging: Print TAC Code ---

static void print_tac_operand(FILE *fp, TacOperand op)
{
    switch (op.type)
    {
    case OP_NONE:
        // fprintf(fp, "");
        break;
    case OP_VAR:
        fprintf(fp, "%s", op.val.name);
        break;
    case OP_TEMP:
        fprintf(fp, "%s", op.val.name);
        break;
    case OP_LIT_INT:
        fprintf(fp, "%d", op.val.int_val);
        break;
    case OP_LIT_FLOAT:
        fprintf(fp, "%f", op.val.float_val);
        break;
    case OP_LIT_CHAR:
        fprintf(fp, "'%c'", op.val.char_val);
        break;
    case OP_LIT_BOOL:
        fprintf(fp, "%s", op.val.int_val ? "true" : "false");
        break;
    case OP_LABEL:
        fprintf(fp, "%s", op.val.name);
        break;
    case OP_STRING_LIT:
        fprintf(fp, "\"%s\"", op.val.name);
        break;
    }
}

void print_tac_code()
{
    printf("\n--- Generated Three-Address Code (TAC) ---\n");
    TacInstruction *current = tac_head;
    while (current)
    {
        printf("%4d: ", current->line_number); // Optional: print line number from source
        switch (current->opcode)
        {
        case TAC_NOP:
            printf("NOP\n");
            break;
        case TAC_ADD:
        case TAC_SUB:
        case TAC_MUL:
        case TAC_DIV:
        case TAC_MOD:
        case TAC_LT:
        case TAC_GT:
        case TAC_LEQ:
        case TAC_GEQ:
        case TAC_EQ:
        case TAC_NEQ:
        case TAC_AND:
        case TAC_OR:
            print_tac_operand(stdout, current->result);
            printf(" = ");
            print_tac_operand(stdout, current->op1);
            switch (current->opcode)
            {
            case TAC_ADD:
                printf(" + ");
                break;
            case TAC_SUB:
                printf(" - ");
                break;
            case TAC_MUL:
                printf(" * ");
                break;
            case TAC_DIV:
                printf(" / ");
                break;
            case TAC_MOD:
                printf(" %% ");
                break;
            case TAC_LT:
                printf(" < ");
                break;
            case TAC_GT:
                printf(" > ");
                break;
            case TAC_LEQ:
                printf(" <= ");
                break;
            case TAC_GEQ:
                printf(" >= ");
                break;
            case TAC_EQ:
                printf(" == ");
                break;
            case TAC_NEQ:
                printf(" != ");
                break;
            case TAC_AND:
                printf(" && ");
                break;
            case TAC_OR:
                printf(" || ");
                break;
            default:
                break; // Should not happen
            }
            print_tac_operand(stdout, current->op2);
            printf("\n");
            break;
        case TAC_ASSIGN:
            print_tac_operand(stdout, current->result);
            printf(" = ");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_LABEL:
            print_tac_operand(stdout, current->result);
            printf(":\n");
            break;
        case TAC_GOTO:
            printf("GOTO ");
            print_tac_operand(stdout, current->result);
            printf("\n");
            break;
        case TAC_IF_GOTO:
            printf("IF ");
            print_tac_operand(stdout, current->result);
            printf(" GOTO ");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_IF_NOT_GOTO:
            printf("IF NOT ");
            print_tac_operand(stdout, current->result);
            printf(" GOTO ");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_NOT:
            print_tac_operand(stdout, current->result);
            printf(" = !");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_UMINUS:
            print_tac_operand(stdout, current->result);
            printf(" = -");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_PRINT:
            printf("PRINT ");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_RETURN:
            printf("RETURN ");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_CALL:
            if (current->result.type != OP_NONE)
            {
                print_tac_operand(stdout, current->result);
                printf(" = ");
            }
            printf("CALL ");
            print_tac_operand(stdout, current->op1); // Function name
            // Parameters are usually handled by separate PARAM instructions before CALL
            printf("\n");
            break;
        case TAC_PARAM:
            printf("PARAM ");
            print_tac_operand(stdout, current->op1);
            printf("\n");
            break;
        case TAC_FUNCTION_BEGIN:
            printf("FUNCTION_BEGIN %s\n", current->op1.val.name);
            break;
        case TAC_FUNCTION_END:
            printf("FUNCTION_END %s\n", current->op1.val.name);
            break;
        case TAC_ARRAY_STORE:                           // result[op1] = op2
            print_tac_operand(stdout, current->result); // Array identifier
            printf("[");
            print_tac_operand(stdout, current->op1); // Index
            printf("] = ");
            print_tac_operand(stdout, current->op2); // Value
            printf("\n");
            break;
        case TAC_ARRAY_LOAD:                            // result = op1[op2]
            print_tac_operand(stdout, current->result); // Destination
            printf(" = ");
            print_tac_operand(stdout, current->op1); // Array identifier
            printf("[");
            print_tac_operand(stdout, current->op2); // Index
            printf("]\n");
            break;
        case TAC_NEW_ARRAY: // result = new type[op1]
            print_tac_operand(stdout, current->result);
            printf(" = NEW ");
            printf("%s[", type_to_string(current->result.data_type)); // Assuming a type_to_string helper
            print_tac_operand(stdout, current->op1);
            printf("]\n");
            break;
        }
        current = current->next;
    }
    printf("------------------------------------------\n");
}

void free_tac_code()
{
    TacInstruction *current = tac_head;
    TacInstruction *next_instr;
    while (current)
    {
        next_instr = current->next;
        // Free dynamically allocated operand names
        if (current->result.type == OP_VAR || current->result.type == OP_TEMP || current->result.type == OP_LABEL || current->result.type == OP_STRING_LIT)
        {
            free(current->result.val.name);
        }
        if (current->op1.type == OP_VAR || current->op1.type == OP_TEMP || current->op1.type == OP_LABEL || current->op1.type == OP_STRING_LIT)
        {
            free(current->op1.val.name);
        }
        if (current->op2.type == OP_VAR || current->op2.type == OP_TEMP || current->op2.type == OP_LABEL || current->op2.type == OP_STRING_LIT)
        {
            free(current->op2.val.name);
        }
        free(current);
        current = next_instr;
    }
    tac_head = NULL;
    tac_tail = NULL;
}