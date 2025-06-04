#ifndef AST_H
#define AST_H

#include <stddef.h> // For size_t
// Ensure symbol_table_types.h is appropriate.
// It should define TypeKind and at least forward declare 'struct Scope;' or define it.
#include "symbol_table_types.h"

// Forward Declarations for Node Structures (struct keyword is good practice here)
typedef struct AstNode AstNode;
typedef struct ProgramNode ProgramNode;
typedef struct ClassDeclarationNode ClassDeclarationNode;
typedef struct MainMethodNode MainMethodNode;
typedef struct StatementListNode StatementListNode;
typedef struct StatementNode StatementNode;
typedef struct VariableDeclarationListNode VariableDeclarationListNode;
typedef struct VariableDeclarationNode VariableDeclarationNode;
typedef struct AssignmentNode AssignmentNode;
typedef struct IfStatementNode IfStatementNode;
typedef struct WhileStatementNode WhileStatementNode;
typedef struct PrintStatementNode PrintStatementNode;
typedef struct ReturnStatementNode ReturnStatementNode;
typedef struct BlockStatementNode BlockStatementNode;
typedef struct ExpressionNode ExpressionNode;
typedef struct LiteralNode LiteralNode;
typedef struct IdentifierNode IdentifierNode;
typedef struct TypeNode TypeNode;
// typedef struct Scope Scope; // Should be handled by symbol_table_types.h

// Structure to temporarily hold parsed class contents (fields and main method)
// before they are assembled into a ClassDeclarationNode.
// This is used in the parser (Bison) to accumulate these parts.
typedef struct ParsedClassContents
{
    struct MainMethodNode *main_method;
    struct VariableDeclarationListNode *var_decls;
} ParsedClassContents;

// --- Node Type Enumeration ---
typedef enum AstNodeType
{
    // Program Structure
    NODE_PROGRAM,
    NODE_CLASS_DECLARATION,
    NODE_MAIN_METHOD,
    NODE_STATEMENT_LIST,
    NODE_VARIABLE_DECLARATION_LIST,

    // Statements (Generic StatementNode will have a 'kind' and a union)
    // These enum values can be used for StatementNode.kind
    NODE_STATEMENT_VAR_DECL_WRAPPER,   // If StatementNode wraps a VariableDeclarationNode
    NODE_STATEMENT_ASSIGNMENT_WRAPPER, // If StatementNode wraps an AssignmentNode
    NODE_STATEMENT_IF_WRAPPER,         // If StatementNode wraps an IfStatementNode
    NODE_STATEMENT_WHILE_WRAPPER,      // If StatementNode wraps a WhileStatementNode
    NODE_STATEMENT_PRINT_WRAPPER,      // If StatementNode wraps a PrintStatementNode
    NODE_STATEMENT_RETURN_WRAPPER,     // If StatementNode wraps a ReturnStatementNode
    NODE_STATEMENT_BLOCK_WRAPPER,      // If StatementNode wraps a BlockStatementNode
    NODE_STATEMENT_EXPRESSION_WRAPPER, // For expression statements

    // Concrete Node types (these are for the actual data structures)
    NODE_VARIABLE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_PRINT_STATEMENT,
    NODE_RETURN_STATEMENT,
    NODE_BLOCK_STATEMENT,

    // Expressions
    NODE_EXPRESSION_BINARY,
    NODE_EXPRESSION_UNARY,
    NODE_EXPRESSION_LITERAL,
    NODE_EXPRESSION_IDENTIFIER,
    NODE_EXPRESSION_ARRAY_ACCESS,
    NODE_EXPRESSION_PARENTHESIZED, // May not be needed if parser handles precedence
    NODE_EXPRESSION_BOOLEAN,       // For TRUE/FALSE literals

    // Literals
    NODE_LITERAL_INTEGER,
    NODE_LITERAL_FLOAT,
    NODE_LITERAL_CHAR,
    NODE_LITERAL_STRING,

    // Types
    NODE_TYPE, // Generic type node, specific kind in TypeNode.kind

    // Identifier
    NODE_IDENTIFIER
} AstNodeType;

// --- Base AST Node Structure ---
struct AstNode
{
    AstNodeType node_type; // Renamed from 'type' to avoid conflict with 'TypeNode' or other uses
    int line_number;
};

// --- Expression Kind Enumeration ---
typedef enum ExpressionKind
{
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_LITERAL,
    EXPR_IDENTIFIER,
    EXPR_ARRAY_ACCESS,
    EXPR_PARENTHESIZED, // For ( expression )
    EXPR_BOOLEAN_TRUE,  // For boolean true literal
    EXPR_BOOLEAN_FALSE  // For boolean false literal
} ExpressionKind;

// --- Literal Kind Enumeration ---
typedef enum LiteralKind
{
    LIT_INTEGER,
    LIT_FLOAT,
    LIT_CHAR,
    LIT_STRING
} LiteralKind;

// --- Statement Kind Enumeration (for the union in StatementNode) ---
typedef enum StatementKind
{
    STMT_KIND_VAR_DECL,
    STMT_KIND_ASSIGNMENT,
    STMT_KIND_IF,
    STMT_KIND_WHILE,
    STMT_KIND_PRINT,
    STMT_KIND_RETURN,
    STMT_KIND_BLOCK,
    STMT_KIND_EXPRESSION // For expression statements, if any
} StatementKind;

// --- Concrete Node Structures ---

struct ProgramNode
{
    AstNode base;
    ClassDeclarationNode *class_decl;
};

struct ClassDeclarationNode
{
    AstNode base;
    IdentifierNode *name;
    MainMethodNode *main_method;
    VariableDeclarationListNode *var_decls; // Field declarations
    struct Scope *associated_scope;         // Scope for this class
};

struct MainMethodNode
{
    AstNode base;
    IdentifierNode *name; // Should be "main"
    BlockStatementNode *body;
    struct Scope *associated_scope; // Scope for this method
};

struct StatementListNode
{
    AstNode base;
    StatementNode *statement;
    StatementListNode *next;
};

// Generic Statement Node (Wrapper)
struct StatementNode // This is what rules like 'statement' in Bison will produce
{
    AstNode base;       // base.node_type would be one of NODE_STATEMENT_..._WRAPPER
    StatementKind kind; // To distinguish the type in the union
    union
    {
        // Pointers to the *actual* statement type nodes
        VariableDeclarationNode *var_decl_data; // If kind is STMT_KIND_VAR_DECL
        AssignmentNode *assignment_data;        // If kind is STMT_KIND_ASSIGNMENT
        IfStatementNode *if_data;               // If kind is STMT_KIND_IF
        WhileStatementNode *while_data;         // If kind is STMT_KIND_WHILE
        PrintStatementNode *print_data;         // If kind is STMT_KIND_PRINT
        ReturnStatementNode *return_data;       // If kind is STMT_KIND_RETURN
        BlockStatementNode *block_data;         // If kind is STMT_KIND_BLOCK
        ExpressionNode *expression_data;        // If kind is STMT_KIND_EXPRESSION
    } data;
};

struct VariableDeclarationListNode
{
    AstNode base;
    VariableDeclarationNode *declaration;
    VariableDeclarationListNode *next;
};

struct VariableDeclarationNode
{
    AstNode base; // base.node_type = NODE_VARIABLE_DECLARATION
    TypeNode *var_type;
    IdentifierNode *identifier;
    ExpressionNode *initializer; // Optional
    int is_final;
};

struct AssignmentNode
{
    AstNode base; // base.node_type = NODE_ASSIGNMENT
    IdentifierNode *target_identifier;
    ExpressionNode *array_index; // Optional: for array assignments like arr[i] = val
    ExpressionNode *value;
};

struct IfStatementNode
{
    AstNode base; // base.node_type = NODE_IF_STATEMENT
    ExpressionNode *condition;
    StatementNode *then_branch; // This should be a generic StatementNode
    StatementNode *else_branch; // Optional, also a generic StatementNode
};

struct WhileStatementNode
{
    AstNode base; // base.node_type = NODE_WHILE_STATEMENT
    ExpressionNode *condition;
    StatementNode *body; // This should be a generic StatementNode
};

struct PrintStatementNode
{
    AstNode base; // base.node_type = NODE_PRINT_STATEMENT
    ExpressionNode *expression;
};

struct ReturnStatementNode
{
    AstNode base;               // base.node_type = NODE_RETURN_STATEMENT
    ExpressionNode *expression; // Optional (for void methods)
};

struct BlockStatementNode
{
    AstNode base; // base.node_type = NODE_BLOCK_STATEMENT
    StatementListNode *statements;
    struct Scope *associated_scope; // Scope for this block
};

struct ExpressionNode
{
    AstNode base; // base.node_type will be one of NODE_EXPRESSION_...
    ExpressionKind kind;
    union
    {
        struct
        { // Binary Expression
            ExpressionNode *left;
            int op_token; // Token for operator (e.g., TOKEN_PLUS from parser)
            ExpressionNode *right;
        } binary_expr;
        struct
        {                 // Unary Expression
            int op_token; // Token for operator (e.g., TOKEN_MINUS, TOKEN_NOT)
            ExpressionNode *operand;
        } unary_expr;
        LiteralNode *literal_expr;
        IdentifierNode *identifier_expr;
        struct
        {                                     // Array Access
            IdentifierNode *array_name_ident; // Changed from array_base to be clearer
            ExpressionNode *index_expr;
        } array_access_expr;
        ExpressionNode *parenthesized_expr_val; // For ( expression )
                                                // For boolean true/false, kind is EXPR_BOOLEAN_TRUE/EXPR_BOOLEAN_FALSE
                                                // no extra data needed beyond 'kind'
    } data;
    TypeKind resolved_type; // Filled by semantic analyzer (from symbol_table_types.h)
};

struct LiteralNode
{
    AstNode base; // base.node_type will be one of NODE_LITERAL_...
    LiteralKind kind;
    union
    {
        int int_val;
        float float_val;
        char char_val;
        char *string_val; // Must be allocated (e.g. strdup) and freed
    } data;
};

struct IdentifierNode
{
    AstNode base; // base.node_type = NODE_IDENTIFIER
    char *name;   // Must be allocated (e.g. strdup) and freed
};

struct TypeNode
{
    AstNode base;  // base.node_type = NODE_TYPE
    TypeKind kind; // e.g. TYPE_INT, TYPE_BOOLEAN, TYPE_INT_ARRAY from symbol_table_types.h
};

// --- AST Creation Function Prototypes ---
ProgramNode *create_program_node(ClassDeclarationNode *class_decl, int line_number);
ClassDeclarationNode *create_class_declaration_node(IdentifierNode *name, MainMethodNode *main_method, VariableDeclarationListNode *var_decls, int line_number);
MainMethodNode *create_main_method_node(IdentifierNode *name, BlockStatementNode *body, int line_number);

StatementListNode *create_statement_list_node(StatementNode *statement, StatementListNode *next_list_item); // Corrected param name
VariableDeclarationListNode *create_variable_declaration_list_node(VariableDeclarationNode *declaration, VariableDeclarationListNode *next_list_item);

// Concrete Node Creation
VariableDeclarationNode *create_variable_declaration_node(TypeNode *var_type, IdentifierNode *identifier, ExpressionNode *initializer, int is_final, int line_number);
AssignmentNode *create_assignment_node(IdentifierNode *target_identifier, ExpressionNode *array_index, ExpressionNode *value, int line_number);
IfStatementNode *create_if_node(ExpressionNode *condition, StatementNode *then_branch, StatementNode *else_branch, int line_number); // Changed from create_if_statement_node to avoid clash
WhileStatementNode *create_while_node(ExpressionNode *condition, StatementNode *body, int line_number);                              // Changed from create_while_statement_node
PrintStatementNode *create_print_node(ExpressionNode *expression, int line_number);                                                  // Changed from create_print_statement_node
ReturnStatementNode *create_return_node(ExpressionNode *expression, int line_number);                                                // Changed from create_return_statement_node
BlockStatementNode *create_block_node(StatementListNode *statements, int line_number);                                               // Changed from create_block_statement_node

// Statement Wrapper Creation Functions (These create generic StatementNode*)
StatementNode *create_variable_declaration_statement(VariableDeclarationNode *var_decl_data, int line_number);
StatementNode *create_assignment_statement(AssignmentNode *assignment_data, int line_number);
StatementNode *create_if_statement_wrapper_node(IfStatementNode *if_data, int line_number);             // To wrap IfStatementNode
StatementNode *create_while_statement_wrapper_node(WhileStatementNode *while_data, int line_number);    // To wrap WhileStatementNode
StatementNode *create_print_statement_wrapper_node(PrintStatementNode *print_data, int line_number);    // To wrap PrintStatementNode
StatementNode *create_return_statement_wrapper_node(ReturnStatementNode *return_data, int line_number); // To wrap ReturnStatementNode
StatementNode *create_block_statement_wrapper_node(BlockStatementNode *block_data, int line_number);    // To wrap BlockStatementNode

// Expression Functions
ExpressionNode *create_binary_expression_node(ExpressionNode *left, int op_token, ExpressionNode *right, int line_number);
ExpressionNode *create_unary_expression_node(int op_token, ExpressionNode *operand, int line_number);
ExpressionNode *create_literal_expression_node(LiteralNode *literal, int line_number);
ExpressionNode *create_identifier_expression_node(IdentifierNode *identifier, int line_number);
ExpressionNode *create_array_access_expression_node(IdentifierNode *array_name_ident, ExpressionNode *index_expr, int line_number);
ExpressionNode *create_parenthesized_expression_node(ExpressionNode *inner_expr, int line_number);
ExpressionNode *create_boolean_literal_expression_node(int is_true, int line_number); // For TRUE/FALSE, e.g., is_true=1 for true

// Other Node Creation Functions
IdentifierNode *create_identifier_node(char *name, int line_number); // Name should be strdup'd by this function
LiteralNode *create_int_literal_node(int value, int line_number);
LiteralNode *create_float_literal_node(float value, int line_number);
LiteralNode *create_char_literal_node(char value, int line_number);
LiteralNode *create_string_literal_node(char *value, int line_number); // Value should be strdup'd by this function
TypeNode *create_type_node(TypeKind kind, int line_number);            // TypeKind from symbol_table_types.h

// --- AST Printing Function Prototypes (for debugging) ---
// (Keep your existing print function prototypes)
void print_ast(ProgramNode *node, int indent);
// ... and so on for all node types ...
const char *get_token_name_from_int(int token); // Changed from get_token_name to avoid potential conflicts

// --- AST Freeing Function Prototypes ---
// (Keep your existing free function prototypes, ensure they handle all members, including strdup'd strings)
void free_ast(ProgramNode *node);
// ... and so on for all node types, ensuring to free sub-nodes and allocated strings ...
void free_parsed_class_contents(ParsedClassContents *pcc); // Add if needed for cleanup on error

#endif // AST_H