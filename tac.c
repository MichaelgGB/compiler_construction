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

// Initialize or reset the TAC generator state
void init_tac_generator()
{
    if (tac_head != NULL) {
        // It's good practice to free existing TAC if re-initializing
        // to prevent memory leaks.
        // printf("Warning: Re-initializing TAC generator. Freeing existing TAC.\n");
        free_tac_code();
    }
    tac_head = NULL;
    tac_tail = NULL;
    temp_counter = 0;
    label_counter = 0;
}

// Helper function to convert TypeKind enum to string for printing
const char *type_to_string(TypeKind type)
{
    switch (type)
    {
    case TYPE_INT:       return "int";
    case TYPE_BOOLEAN:   return "bool";
    case TYPE_VOID:      return "void";
    case TYPE_INT_ARRAY: return "int[]";
    case TYPE_FLOAT:     return "float";
    case TYPE_CHAR:      return "char";
    case TYPE_STRING:    return "string";
    case TYPE_ERROR:     return "error_type"; // Corrected from "error" for consistency
    case TYPE_UNDEFINED: return "undefined_type";
    default:             return "unknown_type";
    }
}

// Create a new TAC instruction
TacInstruction *create_tac_instruction(TacOpCode opcode, TacOperand result, TacOperand op1, TacOperand op2, int line_num)
{
    TacInstruction *instr = (TacInstruction *)malloc(sizeof(TacInstruction));
    if (!instr)
    {
        fprintf(stderr, "Fatal: Memory allocation failed for TAC instruction.\n");
        exit(EXIT_FAILURE);
    }
    instr->opcode = opcode;
    instr->result = result;
    instr->op1 = op1;
    instr->op2 = op2;
    instr->next = NULL;
    // instr->prev = NULL; // This line was removed as 'prev' is not in your TacInstruction struct
    instr->line_number = line_num;
    return instr;
}

// Add a new TAC instruction to the end of the list
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
    op.data_type = TYPE_VOID;
    op.val.name = NULL;
    return op;
}

TacOperand create_tac_var_operand(const char *name, TypeKind type)
{
    TacOperand op;
    op.type = OP_VAR;
    if (name) {
        op.val.name = strdup(name);
        if (!op.val.name) {
            fprintf(stderr, "Fatal: strdup failed for variable name.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Fatal: NULL name provided for variable operand.\n");
        exit(EXIT_FAILURE);
    }
    op.data_type = type;
    return op;
}

TacOperand create_tac_temp_operand(TypeKind type)
{
    TacOperand op;
    op.type = OP_TEMP;
    op.val.name = generate_new_temp_name(); // This should return an allocated string
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
    op.data_type = TYPE_FLOAT;
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
    op.val.int_val = val;
    op.data_type = TYPE_BOOLEAN;
    return op;
}

TacOperand create_tac_string_literal_operand(const char *val)
{
    TacOperand op;
    op.type = OP_STRING_LIT;
    if (val) {
        op.val.name = strdup(val);
        if (!op.val.name) {
            fprintf(stderr, "Fatal: strdup failed for string literal.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        op.val.name = strdup(""); // Handle NULL by creating an empty string
        if (!op.val.name) {
            fprintf(stderr, "Fatal: strdup failed for empty string literal.\n");
            exit(EXIT_FAILURE);
        }
    }
    op.data_type = TYPE_STRING;
    return op;
}

TacOperand create_tac_label_operand(const char *label_name)
{
    TacOperand op;
    op.type = OP_LABEL;
    if (label_name) {
        op.val.name = strdup(label_name);
        if (!op.val.name) {
            fprintf(stderr, "Fatal: strdup failed for label name.\n");
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Fatal: NULL name provided for label operand.\n");
        exit(EXIT_FAILURE);
    }
    op.data_type = TYPE_VOID;
    return op;
}

// --- Name Generation Helpers ---
// These functions now use strdup to return allocated memory,
// consistent with how operands store names.
char *generate_new_temp_name()
{
    char buffer[16]; // Temporary buffer for sprintf
    sprintf(buffer, "_t%d", temp_counter++);
    char *name = strdup(buffer);
    if (!name) {
        fprintf(stderr, "Fatal: strdup failed for temp name.\n");
        exit(EXIT_FAILURE);
    }
    return name;
}

char *generate_new_label_name()
{
    char buffer[16]; // Temporary buffer for sprintf
    sprintf(buffer, "L%d", label_counter++);
    char *name = strdup(buffer);
    if (!name) {
        fprintf(stderr, "Fatal: strdup failed for label name.\n");
        exit(EXIT_FAILURE);
    }
    return name;
}

// --- Debugging: Print TAC Code ---

static void print_tac_operand(FILE *fp, TacOperand op)
{
    switch (op.type)
    {
    case OP_NONE:
        // fprintf(fp, "_"); // Optionally print a placeholder
        break;
    case OP_VAR:
    case OP_TEMP:
    case OP_LABEL:
        if (op.val.name) fprintf(fp, "%s", op.val.name);
        else fprintf(fp, "<NULL_NAME_OP>"); // Should not happen
        break;
    case OP_STRING_LIT:
        if (op.val.name) fprintf(fp, "\"%s\"", op.val.name);
        else fprintf(fp, "<NULL_STRING_LIT>"); // Should not happen
        break;
    case OP_LIT_INT:
        fprintf(fp, "%d", op.val.int_val);
        break;
    case OP_LIT_FLOAT:
        fprintf(fp, "%f", op.val.float_val);
        break;
    case OP_LIT_CHAR:
        if (op.val.char_val >= 32 && op.val.char_val < 127) fprintf(fp, "'%c'", op.val.char_val);
        else fprintf(fp, "'\\x%02x'", (unsigned char)op.val.char_val); // Print non-printable as hex
        break;
    case OP_LIT_BOOL:
        fprintf(fp, "%s", op.val.int_val ? "true" : "false");
        break;
    default:
        fprintf(fp, "<UNKNOWN_OPERAND>");
        break;
    }
}

void print_tac_code()
{
    printf("\n--- Generated Three-Address Code (TAC) ---\n");
    TacInstruction *current = tac_head;
    int line_counter = 0; // Use this for TAC line numbers for consistency

    while (current)
    {
        // Using TAC line number instead of original source line for this printout
        printf("%4d: ", line_counter++); 
        // Or, if you want to keep the original source line:
        // printf("%4d: ", current->line_number); 

        switch (current->opcode)
        {
        case TAC_NOP: printf("NOP"); break;
        case TAC_ADD: case TAC_SUB: case TAC_MUL: case TAC_DIV: case TAC_MOD:
        case TAC_LT:  case TAC_GT:  case TAC_LEQ: case TAC_GEQ:
        case TAC_EQ:  case TAC_NEQ: case TAC_AND: case TAC_OR:
            print_tac_operand(stdout, current->result); printf(" = ");
            print_tac_operand(stdout, current->op1);
            switch (current->opcode) {
                case TAC_ADD: printf(" + "); break; case TAC_SUB: printf(" - "); break;
                case TAC_MUL: printf(" * "); break; case TAC_DIV: printf(" / "); break;
                case TAC_MOD: printf(" %% "); break; case TAC_LT:  printf(" < "); break;
                case TAC_GT:  printf(" > "); break; case TAC_LEQ: printf(" <= "); break;
                case TAC_GEQ: printf(" >= "); break; case TAC_EQ:  printf(" == "); break;
                case TAC_NEQ: printf(" != "); break; case TAC_AND: printf(" && "); break;
                case TAC_OR:  printf(" || "); break;
                default: break; 
            }
            print_tac_operand(stdout, current->op2);
            break;
        case TAC_ASSIGN:
            print_tac_operand(stdout, current->result); printf(" = ");
            print_tac_operand(stdout, current->op1);
            break;
        case TAC_LABEL: // Label name is in result
            print_tac_operand(stdout, current->result); printf(":");
            break;
        case TAC_GOTO: // Target label is in result
            printf("GOTO "); print_tac_operand(stdout, current->result);
            break;
        case TAC_IF_GOTO: // Condition in op1, target label in result
            printf("IF "); print_tac_operand(stdout, current->op1);    
            printf(" GOTO "); print_tac_operand(stdout, current->result); 
            break;
        case TAC_IF_NOT_GOTO: // Condition in op1, target label in result
            printf("IF NOT "); print_tac_operand(stdout, current->op1);    
            printf(" GOTO "); print_tac_operand(stdout, current->result); 
            break;
        case TAC_NOT:
            print_tac_operand(stdout, current->result); printf(" = !");
            print_tac_operand(stdout, current->op1);
            break;
        case TAC_UMINUS:
            print_tac_operand(stdout, current->result); printf(" = -");
            print_tac_operand(stdout, current->op1);
            break;
        case TAC_PRINT: // Value to print is in op1
            printf("PRINT "); print_tac_operand(stdout, current->op1); 
            break;
        case TAC_RETURN: // Value to return is in op1 (or OP_NONE)
            printf("RETURN "); print_tac_operand(stdout, current->op1); 
            break;
        case TAC_CALL: // Function name in op1, return value (if any) in result
            if (current->result.type != OP_NONE) {
                print_tac_operand(stdout, current->result); printf(" = ");
            }
            printf("CALL "); print_tac_operand(stdout, current->op1); 
            break;
        case TAC_PARAM: // Parameter value in op1
            printf("PARAM "); print_tac_operand(stdout, current->op1);
            break;
        case TAC_FUNCTION_BEGIN: // Function name in op1            
            if(current->op1.type == OP_LABEL && current->op1.val.name) printf("%s:", current->op1.val.name); 
            else printf("<ERROR_FUNC_BEGIN>:"); // Should be a label operand
            break;
        case TAC_FUNCTION_END: 
            // No standard TAC output, but could be a comment
            // if(current->op1.type == OP_LABEL && current->op1.val.name) printf("; End %s", current->op1.val.name);
            break;
        case TAC_ARRAY_STORE: // result_array[op1_index] = op2_value
            print_tac_operand(stdout, current->result); printf("[");
            print_tac_operand(stdout, current->op1);    printf("] = ");
            print_tac_operand(stdout, current->op2);    
            break;
        case TAC_ARRAY_LOAD:  // result_dest = op1_array[op2_index] 
            print_tac_operand(stdout, current->result); printf(" = ");
            print_tac_operand(stdout, current->op1);    printf("[");
            print_tac_operand(stdout, current->op2);    printf("]");
            break;
        case TAC_NEW_ARRAY: // result_array = NEW_ARRAY element_type(in result.data_type), op1_size
            print_tac_operand(stdout, current->result);
            printf(" = NEW_ARRAY %s, ", type_to_string(current->result.data_type)); 
            print_tac_operand(stdout, current->op1);                                
            break;
        default:
            printf("<UNKNOWN_OPCODE_%d>", current->opcode);
            break;
        }
        printf("\n");
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

        if ((current->result.type == OP_VAR || current->result.type == OP_TEMP ||
             current->result.type == OP_LABEL || current->result.type == OP_STRING_LIT) &&
            current->result.val.name)
        {
            free(current->result.val.name);
            current->result.val.name = NULL; 
        }
        if ((current->op1.type == OP_VAR || current->op1.type == OP_TEMP ||
             current->op1.type == OP_LABEL || current->op1.type == OP_STRING_LIT) &&
            current->op1.val.name)
        {
            free(current->op1.val.name);
            current->op1.val.name = NULL;
        }
        if ((current->op2.type == OP_VAR || current->op2.type == OP_TEMP ||
             current->op2.type == OP_LABEL || current->op2.type == OP_STRING_LIT) &&
            current->op2.val.name)
        {
            free(current->op2.val.name);
            current->op2.val.name = NULL;
        }

        free(current);
        current = next_instr;
    }
    tac_head = NULL;
    tac_tail = NULL;
}