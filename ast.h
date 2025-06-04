#ifndef AST_H
#define AST_H

#include <stddef.h> // For size_t

// --- Forward Declarations for linked list nodes (VERY IMPORTANT for union/struct definitions) ---
typedef struct StatementListNode StatementListNode;
typedef struct VariableDeclarationListNode VariableDeclarationListNode;
typedef struct ProgramNode ProgramNode;
typedef struct ClassDeclarationNode ClassDeclarationNode;
typedef struct MainMethodNode MainMethodNode;
typedef struct StatementNode StatementNode;
typedef struct VariableDeclarationNode VariableDeclarationNode;
typedef struct AssignmentNode AssignmentNode;
typedef struct IfStatementNode IfStatementNode;
typedef struct WhileStatementNode WhileStatementNode;
typedef struct PrintStatementNode PrintStatementNode;
typedef struct ReturnStatementNode ReturnStatementNode;
typedef struct BlockStatementNode BlockStatementNode;
typedef struct ExpressionNode ExpressionNode;
typedef struct IdentifierNode IdentifierNode;
typedef struct LiteralNode LiteralNode;
typedef struct TypeNode TypeNode;
typedef struct ClassBodyContent ClassBodyContent;

// --- Enum for general AST Node Types ---
typedef enum
{
    NODE_PROGRAM,
    NODE_CLASS_DECLARATION,
    NODE_MAIN_METHOD,
    NODE_STATEMENT_LIST,
    NODE_STATEMENT_VAR_DECL, // Specific types of statements
    NODE_STATEMENT_ASSIGNMENT,
    NODE_STATEMENT_IF,
    NODE_STATEMENT_WHILE,
    NODE_STATEMENT_PRINT,
    NODE_STATEMENT_RETURN,
    NODE_STATEMENT_BLOCK,
    NODE_VARIABLE_DECLARATION,
    NODE_VARIABLE_DECLARATION_LIST,
    NODE_ASSIGNMENT,
    NODE_IF_STATEMENT,
    NODE_WHILE_STATEMENT,
    NODE_PRINT_STATEMENT,
    NODE_RETURN_STATEMENT, // Used for the ReturnStatementNode struct itself
    NODE_BLOCK_STATEMENT,
    NODE_EXPRESSION_BINARY,
    NODE_EXPRESSION_UNARY,
    NODE_EXPRESSION_LITERAL,
    NODE_EXPRESSION_IDENTIFIER,
    NODE_EXPRESSION_ARRAY_ACCESS,
    NODE_EXPRESSION_PARENTHESIZED,
    NODE_EXPRESSION_BOOLEAN, // A general boolean expression (TRUE/FALSE)
    NODE_IDENTIFIER,
    NODE_LITERAL_INTEGER,
    NODE_LITERAL_FLOAT,
    NODE_LITERAL_CHAR,
    NODE_LITERAL_STRING,
    NODE_TYPE_INT,
    NODE_TYPE_CHAR,
    NODE_TYPE_BOOLEAN,
    NODE_TYPE_INT_ARRAY
} AstNodeType;

// --- Enum for Statement Kinds (if you have one) ---
typedef enum
{
    STMT_VAR_DECL,
    STMT_ASSIGNMENT,
    STMT_IF,
    STMT_WHILE,
    STMT_PRINT,
    STMT_RETURN,
    STMT_BLOCK,
    STMT_EXPRESSION // If an expression can be a standalone statement
} StatementKind;

// --- Enum for Expression Kinds ---
typedef enum
{
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_LITERAL,
    EXPR_IDENTIFIER,
    EXPR_ARRAY_ACCESS,
    EXPR_PARENTHESIZED,
    EXPR_TRUE, // For boolean true literal
    EXPR_FALSE // For boolean false literal
} ExpressionKind;

// --- Enum for Literal Kinds ---
typedef enum
{
    LIT_INTEGER,
    LIT_FLOAT,
    LIT_CHAR,
    LIT_STRING
} LiteralKind;

// --- Enum for Type Kinds (for variable types, return types, etc.) ---
typedef enum
{
    TYPE_INT,
    TYPE_CHAR,
    TYPE_BOOLEAN,
    TYPE_INT_ARRAY,
    TYPE_FLOAT,  // Added
    TYPE_STRING, // Added
    TYPE_VOID,   // Added (useful for function returns, expected return type)
    TYPE_ERROR   // Added (to signify an error in type checking)
} TypeKind;

// --- Base Node Structure ---
typedef struct AstNode
{
    AstNodeType type; // The specific type of AST node
    int line_number;  // Line number from source code
    // Add common fields here, if any
} AstNode;

// Function to create a generic AST node (used internally by specific node creation functions)
AstNode *create_node(AstNodeType type, size_t size, int line_number);

// --- Specific AST Node Structures ---

typedef struct ProgramNode
{
    AstNode base;
    ClassDeclarationNode *class_decl;
} ProgramNode;

typedef struct ClassDeclarationNode
{
    AstNode base;
    IdentifierNode *name;
    MainMethodNode *main_method;
    VariableDeclarationListNode *var_decls; // List of variable declarations in the class body
} ClassDeclarationNode;

typedef struct MainMethodNode
{
    AstNode base;
    IdentifierNode *name;
    BlockStatementNode *body;
} MainMethodNode;

typedef struct StatementNode
{
    AstNode base;
    StatementKind kind; // Specific kind of statement
    union
    {
        VariableDeclarationNode *var_decl_stmt; // For a standalone variable declaration as a statement
        AssignmentNode *assignment_stmt;
        IfStatementNode *if_stmt;
        WhileStatementNode *while_stmt;
        PrintStatementNode *print_stmt;
        ReturnStatementNode *return_stmt;
        BlockStatementNode *block_stmt;
        ExpressionNode *expression_stmt; // If an expression can be a statement itself
    } data;
} StatementNode;

typedef struct StatementListNode
{
    AstNode base;
    StatementNode *statement;
    struct StatementListNode *next;
} StatementListNode;

typedef struct VariableDeclarationNode
{
    AstNode base;
    TypeNode *var_type;
    IdentifierNode *identifier;
    ExpressionNode *initializer; // Optional: for declarations with initialization
    int is_final;                // 1 if final, 0 otherwise
} VariableDeclarationNode;

typedef struct VariableDeclarationListNode
{
    AstNode base;
    VariableDeclarationNode *declaration;
    struct VariableDeclarationListNode *next;
} VariableDeclarationListNode;

typedef struct AssignmentNode
{
    AstNode base;
    IdentifierNode *target_identifier;
    ExpressionNode *array_index; // NULL if not array assignment
    ExpressionNode *value;
} AssignmentNode;

typedef struct IfStatementNode
{
    AstNode base;
    ExpressionNode *condition;
    StatementNode *then_branch;
    StatementNode *else_branch; // NULL if no else
} IfStatementNode;

typedef struct WhileStatementNode
{
    AstNode base;
    ExpressionNode *condition;
    StatementNode *body;
} WhileStatementNode;

typedef struct PrintStatementNode
{
    AstNode base;
    ExpressionNode *expression;
} PrintStatementNode;

typedef struct ReturnStatementNode
{
    AstNode base;
    ExpressionNode *expression; // The expression being returned
} ReturnStatementNode;

typedef struct BlockStatementNode
{
    AstNode base;
    StatementListNode *statements; // List of statements in the block
} BlockStatementNode;

typedef struct ExpressionNode
{
    AstNode base;
    ExpressionKind kind; // Specific kind of expression
    TypeKind resolved_type;
    union
    {
        struct
        {
            ExpressionNode *left;
            int op; // Token type (e.g., TOKEN_PLUS)
            ExpressionNode *right;
        } binary_expr;
        struct
        {
            int op; // Token type (e.g., TOKEN_MINUS, TOKEN_NOT)
            ExpressionNode *operand;
        } unary_expr;
        LiteralNode *literal_expr;
        IdentifierNode *identifier_expr;
        struct
        {
            IdentifierNode *array_base;
            ExpressionNode *index;
        } array_access_expr;
        ExpressionNode *parenthesized_expr; // For (expression)
        // No specific data for boolean literals, their kind is enough
    } data;
} ExpressionNode;

typedef struct IdentifierNode
{
    AstNode base;
    char *name;
} IdentifierNode;

typedef struct LiteralNode
{
    AstNode base;
    LiteralKind kind;
    union
    {
        int int_val;
        float float_val;
        char char_val;
        char *string_val;
    } data;
} LiteralNode;

typedef struct TypeNode
{
    AstNode base;
    TypeKind kind;
} TypeNode;

// Struct to hold combined content of class body (for Bison's semantic value)
struct ClassBodyContent
{ // Defined with a tag
    MainMethodNode *main_method;
    VariableDeclarationListNode *var_declarations;
};

// --- AST Node Creation Functions ---
ProgramNode *create_program_node(ClassDeclarationNode *class_decl, int line_number);
ClassDeclarationNode *create_class_declaration_node(IdentifierNode *name, MainMethodNode *main_method, VariableDeclarationListNode *var_decls, int line_number);
MainMethodNode *create_main_method_node(IdentifierNode *name, BlockStatementNode *body, int line_number);

StatementNode *create_variable_declaration_statement(VariableDeclarationNode *var_decl, int line_number);
StatementNode *create_assignment_statement(AssignmentNode *assignment, int line_number);
StatementNode *create_if_statement_node(IfStatementNode *if_stmt, int line_number);
StatementNode *create_while_statement_node(WhileStatementNode *while_stmt, int line_number);
StatementNode *create_print_statement_node(PrintStatementNode *print_stmt, int line_number);
StatementNode *create_return_statement_node(ReturnStatementNode *return_stmt, int line_number); // Expects ReturnStatementNode*
StatementNode *create_block_statement_node(BlockStatementNode *block_stmt, int line_number);

StatementListNode *create_statement_list_node(StatementNode *statement, StatementListNode *next);

VariableDeclarationNode *create_variable_declaration_node(TypeNode *var_type, IdentifierNode *identifier, ExpressionNode *initializer, int is_final, int line_number);
VariableDeclarationListNode *create_variable_declaration_list_node(VariableDeclarationNode *declaration, VariableDeclarationListNode *next);

AssignmentNode *create_assignment_node(IdentifierNode *target_identifier, ExpressionNode *array_index, ExpressionNode *value, int line_number);
IfStatementNode *create_if_node(ExpressionNode *condition, StatementNode *then_branch, StatementNode *else_branch, int line_number);
WhileStatementNode *create_while_node(ExpressionNode *condition, StatementNode *body, int line_number);
PrintStatementNode *create_print_node(ExpressionNode *expression, int line_number);
ReturnStatementNode *create_return_node(ExpressionNode *expression, int line_number); // NEW: Creates the actual ReturnStatementNode
BlockStatementNode *create_block_node(StatementListNode *statements, int line_number);

ExpressionNode *create_binary_expression_node(ExpressionNode *left, int op, ExpressionNode *right, int line_number);
ExpressionNode *create_unary_expression_node(int op, ExpressionNode *operand, int line_number);
ExpressionNode *create_literal_expression_node(LiteralNode *literal, int line_number);
ExpressionNode *create_identifier_expression_node(IdentifierNode *identifier, int line_number);
ExpressionNode *create_array_access_expression_node(IdentifierNode *array_base, ExpressionNode *index, int line_number);
ExpressionNode *create_parenthesized_expression_node(ExpressionNode *inner_expr, int line_number);
ExpressionNode *create_boolean_expression_node(int token_type, int line_number); // Uses TOKEN_TRUE/TOKEN_FALSE

IdentifierNode *create_identifier_node(char *name, int line_number);

LiteralNode *create_int_literal_node(int value, int line_number);
LiteralNode *create_float_literal_node(float value, int line_number);
LiteralNode *create_char_literal_node(char value, int line_number);
LiteralNode *create_string_literal_node(char *value, int line_number);

TypeNode *create_type_node(TypeKind kind, int line_number);

// --- AST Printing Functions ---
void print_ast(ProgramNode *node, int indent);
void print_program_node(ProgramNode *node, int indent);
void print_class_declaration(ClassDeclarationNode *node, int indent);
void print_main_method(MainMethodNode *node, int indent);
void print_statements(StatementListNode *list, int indent);
void print_statement(StatementNode *node, int indent);
void print_block_statement_node(BlockStatementNode *node, int indent);
void print_variable_declaration_node(VariableDeclarationNode *node, int indent);
void print_variable_declaration_list(VariableDeclarationListNode *list, int indent); // Added declaration
void print_assignment_node(AssignmentNode *node, int indent);
void print_if_statement_node(IfStatementNode *node, int indent);
void print_while_statement_node(WhileStatementNode *node, int indent);
void print_print_statement_node(PrintStatementNode *node, int indent);
void print_return_statement_node(ReturnStatementNode *node, int indent);
void print_expression(ExpressionNode *node, int indent);
void print_identifier(IdentifierNode *node, int indent);
void print_literal(LiteralNode *node, int indent);
void print_type(TypeNode *node, int indent);
void print_indent(int indent); // Helper function

// --- AST Memory Management Functions ---
void free_ast(ProgramNode *node);
void free_statement_list_node(StatementListNode *node);
void free_statement_node(StatementNode *node);
void free_variable_declaration_node(VariableDeclarationNode *node);
void free_variable_declaration_list_node(VariableDeclarationListNode *node);
void free_assignment_node(AssignmentNode *node);
void free_if_statement_node(IfStatementNode *node);
void free_while_statement_node(WhileStatementNode *node);
void free_print_statement_node(PrintStatementNode *node);
void free_return_statement_node(ReturnStatementNode *node);
void free_block_statement_node(BlockStatementNode *node);
void free_expression_node(ExpressionNode *node);
void free_identifier_node(IdentifierNode *node);
void free_literal_node(LiteralNode *node);
void free_type_node(TypeNode *node);
void free_main_method_node(MainMethodNode *node);
void free_class_declaration_node(ClassDeclarationNode *node);

#endif // AST_H