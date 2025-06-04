#ifndef TAC_H
#define TAC_H

#include "ast.h" // Include AST definitions for TypeKind etc.

// Define opcodes for Three-Address Code instructions
typedef enum
{
    TAC_NOP, // No operation (useful for placeholder/debugging)
    TAC_ADD, // result = op1 + op2
    TAC_SUB, // result = op1 - op2
    TAC_MUL, // result = op1 * op2
    TAC_DIV, // result = op1 / op2
    TAC_MOD, // result = op1 % op2

    TAC_ASSIGN, // result = op1 (e.g., x = y, x = 5)

    TAC_LABEL,       // Define a label (e.g., L1:)
    TAC_GOTO,        // Unconditional jump (goto L1)
    TAC_IF_GOTO,     // Conditional jump (if op1 goto L1)
    TAC_IF_NOT_GOTO, // Conditional jump (if not op1 goto L1)

    TAC_LT,  // result = op1 < op2
    TAC_GT,  // result = op1 > op2
    TAC_LEQ, // result = op1 <= op2
    TAC_GEQ, // result = op1 >= op2
    TAC_EQ,  // result = op1 == op2
    TAC_NEQ, // result = op1 != op2

    TAC_AND,    // result = op1 && op2
    TAC_OR,     // result = op1 || op2
    TAC_NOT,    // result = !op1 (unary)
    TAC_UMINUS, // result = -op1 (unary minus)

    TAC_PRINT,  // print op1
    TAC_RETURN, // return op1 (for main, or other functions later)

    TAC_CALL,           // result = call op1 (function name) with op2 (arg list)
    TAC_PARAM,          // pass op1 as a parameter to a function call
    TAC_FUNCTION_BEGIN, // Marks the start of a function's TAC block
    TAC_FUNCTION_END,   // Marks the end of a function's TAC block

    TAC_ARRAY_STORE, // result[op1] = op2
    TAC_ARRAY_LOAD,  // result = op1[op2]

    // Memory allocation (if supporting 'new int[N]')
    TAC_NEW_ARRAY, // result = new Type[op1]
} TacOpCode;

// Types of operands in TAC
typedef enum
{
    OP_NONE,       // No operand (e.g., for TAC_NOP or result of GOTO)
    OP_VAR,        // A variable (local or global)
    OP_TEMP,       // A compiler-generated temporary variable
    OP_LIT_INT,    // Integer literal
    OP_LIT_FLOAT,  // Float literal
    OP_LIT_CHAR,   // Char literal
    OP_LIT_BOOL,   // Boolean literal (0 for false, 1 for true)
    OP_LABEL,      // A label reference (for jumps or defining a label)
    OP_STRING_LIT, // String literal (for print or later features)
} TacOperandType;

// Structure for a single operand in TAC
typedef struct TacOperand
{
    TacOperandType type;
    union
    {
        char *name;      // For OP_VAR, OP_TEMP, OP_LABEL, OP_STRING_LIT
        int int_val;     // For OP_LIT_INT, OP_LIT_BOOL
        float float_val; // For OP_LIT_FLOAT
        char char_val;   // For OP_LIT_CHAR
    } val;
    TypeKind data_type; // The resolved type of the operand (e.g., TYPE_INT)
} TacOperand;

// Structure for a Three-Address Code instruction
typedef struct TacInstruction
{
    TacOpCode opcode;
    TacOperand result;           // Destination
    TacOperand op1;              // First operand
    TacOperand op2;              // Second operand
    struct TacInstruction *next; // Pointer to the next instruction in the list
    int line_number;             // For debugging and error reporting
} TacInstruction;

// Global head and tail of the TAC instruction list
extern TacInstruction *tac_head;
extern TacInstruction *tac_tail;

// Functions for TAC generation
void init_tac_generator();
TacInstruction *create_tac_instruction(TacOpCode opcode, TacOperand result, TacOperand op1, TacOperand op2, int line_num);
void emit_tac(TacOpCode opcode, TacOperand result, TacOperand op1, TacOperand op2, int line_num);

// Functions to create different types of operands
TacOperand create_tac_none_operand();
TacOperand create_tac_var_operand(const char *name, TypeKind type);
TacOperand create_tac_temp_operand(TypeKind type);
TacOperand create_tac_int_literal_operand(int val);
TacOperand create_tac_float_literal_operand(float val);
TacOperand create_tac_char_literal_operand(char val);
TacOperand create_tac_bool_literal_operand(int val); // 0 for false, 1 for true
TacOperand create_tac_string_literal_operand(const char *val);
TacOperand create_tac_label_operand(const char *label_name);
const char *type_to_string(TypeKind type);

// Functions for generating unique names
char *generate_new_temp_name();
char *generate_new_label_name();

// Function to print the generated TAC for debugging
void print_tac_code();
void free_tac_code(); // To free allocated memory

#endif // TAC_H