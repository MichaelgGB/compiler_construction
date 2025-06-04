#include "ast.h"
#include "miniJava.tab.h" // For TOKEN_... definitions (ensure this path is correct)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This function was in ast.h as get_token_name_from_int.
// If miniJava.tab.h provides yytname (via %token-table in .y), you might prefer that.
// For now, using the manual switch as provided in your original ast.c,
// but matching the name from ast.h.
const char *get_token_name_from_int(int token)
{
    // This switch statement should ideally use the token values defined
    // by Bison in miniJava.tab.h.
    switch (token)
    {
    case TOKEN_INT_LIT:
        return "INT_LITERAL";
    case TOKEN_FLOAT_LIT:
        return "FLOAT_LITERAL";
    case TOKEN_CHAR_LIT:
        return "CHAR_LITERAL";
    case TOKEN_STRING_LIT:
        return "STRING_LITERAL";
    case TOKEN_ID:
        return "IDENTIFIER";
    case TOKEN_CLASS:
        return "CLASS";
    case TOKEN_PUBLIC:
        return "PUBLIC";
    case TOKEN_STATIC:
        return "STATIC";
    case TOKEN_VOID:
        return "VOID";
    case TOKEN_INT:
        return "INT";
    case TOKEN_CHAR:
        return "CHAR";
    case TOKEN_BOOLEAN:
        return "BOOLEAN";
    case TOKEN_IF:
        return "IF";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_WHILE:
        return "WHILE";
    case TOKEN_RETURN:
        return "RETURN";
    case TOKEN_TRUE:
        return "TRUE";
    case TOKEN_FALSE:
        return "FALSE";
    case TOKEN_FINAL:
        return "FINAL";
    case TOKEN_PRINT:
        return "PRINT";
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_MULTIPLY:
        return "MULTIPLY";
    case TOKEN_DIVIDE:
        return "DIVIDE";
    case TOKEN_MOD:
        return "MOD";
    case TOKEN_EQ:
        return "EQUAL";
    case TOKEN_NEQ:
        return "NOT_EQUAL";
    case TOKEN_LT:
        return "LESS_THAN";
    case TOKEN_GT:
        return "GREATER_THAN";
    case TOKEN_LEQ:
        return "LESS_EQUAL";
    case TOKEN_GEQ:
        return "GREATER_EQUAL";
    case TOKEN_AND:
        return "AND";
    case TOKEN_OR:
        return "OR";
    case TOKEN_NOT:
        return "NOT";
    case TOKEN_ASSIGN:
        return "ASSIGN";
    case TOKEN_LPAREN:
        return "LEFT_PAREN";
    case TOKEN_RPAREN:
        return "RIGHT_PAREN";
    case TOKEN_LBRACE:
        return "LEFT_BRACE";
    case TOKEN_RBRACE:
        return "RIGHT_BRACE";
    case TOKEN_LBRACKET:
        return "LEFT_BRACKET";
    case TOKEN_RBRACKET:
        return "RIGHT_BRACKET";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_COMMA:
        return "COMMA";
    // Add other tokens if necessary
    default:
        // If yytname is available and %token-table was used in Bison:
        // const char* name = yysymbol_name(token);
        // if (name && strncmp(name, "yy", 2) != 0 && strcmp(name, "$end") != 0) return name;
        return "UNKNOWN_TOKEN";
    }
}

// --- Helper for creating base AST node ---
AstNode *create_node(AstNodeType ast_node_type, size_t size, int line_number)
{
    AstNode *node = (AstNode *)calloc(1, size);
    if (!node)
    {
        perror("Failed to allocate AST node");
        exit(EXIT_FAILURE);
    }
    node->node_type = ast_node_type; // CORRECTED: was node->type
    node->line_number = line_number;
    return node;
}

// --- Creation Functions ---

ProgramNode *create_program_node(ClassDeclarationNode *class_decl, int line_number)
{
    ProgramNode *node = (ProgramNode *)create_node(NODE_PROGRAM, sizeof(ProgramNode), line_number);
    node->class_decl = class_decl;
    return node;
}

ClassDeclarationNode *create_class_declaration_node(IdentifierNode *name, MainMethodNode *main_method, VariableDeclarationListNode *var_decls, int line_number)
{
    ClassDeclarationNode *node = (ClassDeclarationNode *)create_node(NODE_CLASS_DECLARATION, sizeof(ClassDeclarationNode), line_number);
    node->name = name;
    node->main_method = main_method;
    node->var_decls = var_decls;
    node->associated_scope = NULL; // Initialize scope
    return node;
}

MainMethodNode *create_main_method_node(IdentifierNode *name, BlockStatementNode *body, int line_number)
{
    MainMethodNode *node = (MainMethodNode *)create_node(NODE_MAIN_METHOD, sizeof(MainMethodNode), line_number);
    node->name = name;
    node->body = body;
    node->associated_scope = NULL; // Initialize scope
    return node;
}

StatementListNode *create_statement_list_node(StatementNode *statement, StatementListNode *next_list_item)
{
    int line = 0;
    if (statement)
        line = statement->base.line_number;
    else if (next_list_item)
        line = next_list_item->base.line_number;

    StatementListNode *node = (StatementListNode *)create_node(NODE_STATEMENT_LIST, sizeof(StatementListNode), line);
    node->statement = statement;
    node->next = next_list_item;
    return node;
}

VariableDeclarationNode *create_variable_declaration_node(TypeNode *var_type, IdentifierNode *identifier, ExpressionNode *initializer, int is_final, int line_number)
{
    VariableDeclarationNode *node = (VariableDeclarationNode *)create_node(NODE_VARIABLE_DECLARATION, sizeof(VariableDeclarationNode), line_number);
    node->var_type = var_type;
    node->identifier = identifier;
    node->initializer = initializer;
    node->is_final = is_final;
    return node;
}

VariableDeclarationListNode *create_variable_declaration_list_node(VariableDeclarationNode *declaration, VariableDeclarationListNode *next_list_item)
{
    int line = 0;
    if (declaration)
        line = declaration->base.line_number;
    else if (next_list_item)
        line = next_list_item->base.line_number;

    VariableDeclarationListNode *node = (VariableDeclarationListNode *)create_node(NODE_VARIABLE_DECLARATION_LIST, sizeof(VariableDeclarationListNode), line);
    node->declaration = declaration;
    node->next = next_list_item;
    return node;
}

AssignmentNode *create_assignment_node(IdentifierNode *target_identifier, ExpressionNode *array_index, ExpressionNode *value, int line_number)
{
    AssignmentNode *node = (AssignmentNode *)create_node(NODE_ASSIGNMENT, sizeof(AssignmentNode), line_number);
    node->target_identifier = target_identifier;
    node->array_index = array_index;
    node->value = value;
    return node;
}

IfStatementNode *create_if_node(ExpressionNode *condition, StatementNode *then_branch, StatementNode *else_branch, int line_number)
{
    IfStatementNode *node = (IfStatementNode *)create_node(NODE_IF_STATEMENT, sizeof(IfStatementNode), line_number);
    node->condition = condition;
    node->then_branch = then_branch;
    node->else_branch = else_branch;
    return node;
}

WhileStatementNode *create_while_node(ExpressionNode *condition, StatementNode *body, int line_number)
{
    WhileStatementNode *node = (WhileStatementNode *)create_node(NODE_WHILE_STATEMENT, sizeof(WhileStatementNode), line_number);
    node->condition = condition;
    node->body = body;
    return node;
}

PrintStatementNode *create_print_node(ExpressionNode *expression, int line_number)
{
    PrintStatementNode *node = (PrintStatementNode *)create_node(NODE_PRINT_STATEMENT, sizeof(PrintStatementNode), line_number);
    node->expression = expression;
    return node;
}

ReturnStatementNode *create_return_node(ExpressionNode *expression, int line_number)
{
    ReturnStatementNode *node = (ReturnStatementNode *)create_node(NODE_RETURN_STATEMENT, sizeof(ReturnStatementNode), line_number);
    node->expression = expression;
    return node;
}

BlockStatementNode *create_block_node(StatementListNode *statements, int line_number)
{
    BlockStatementNode *node = (BlockStatementNode *)create_node(NODE_BLOCK_STATEMENT, sizeof(BlockStatementNode), line_number);
    node->statements = statements;
    node->associated_scope = NULL; // Initialize scope
    return node;
}

// --- Statement Wrapper Creation Functions ---
StatementNode *create_variable_declaration_statement(VariableDeclarationNode *var_decl_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_VAR_DECL_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_VAR_DECL;                                                                                         // CORRECTED
    node->data.var_decl_data = var_decl_data;                                                                                // CORRECTED
    return node;
}

StatementNode *create_assignment_statement(AssignmentNode *assignment_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_ASSIGNMENT_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_ASSIGNMENT;                                                                                         // CORRECTED
    node->data.assignment_data = assignment_data;                                                                              // CORRECTED
    return node;
}

StatementNode *create_if_statement_wrapper_node(IfStatementNode *if_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_IF_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_IF;                                                                                         // CORRECTED
    node->data.if_data = if_data;                                                                                      // CORRECTED
    return node;
}

StatementNode *create_while_statement_wrapper_node(WhileStatementNode *while_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_WHILE_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_WHILE;                                                                                         // CORRECTED
    node->data.while_data = while_data;                                                                                   // CORRECTED
    return node;
}

StatementNode *create_print_statement_wrapper_node(PrintStatementNode *print_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_PRINT_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_PRINT;                                                                                         // CORRECTED
    node->data.print_data = print_data;                                                                                   // CORRECTED
    return node;
}

StatementNode *create_return_statement_wrapper_node(ReturnStatementNode *return_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_RETURN_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_RETURN;                                                                                         // CORRECTED
    node->data.return_data = return_data;                                                                                  // CORRECTED
    return node;
}

StatementNode *create_block_statement_wrapper_node(BlockStatementNode *block_data, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_BLOCK_WRAPPER, sizeof(StatementNode), line_number); // CORRECTED
    node->kind = STMT_KIND_BLOCK;                                                                                         // CORRECTED
    node->data.block_data = block_data;                                                                                   // CORRECTED
    return node;
}

// --- Expression Functions ---
ExpressionNode *create_binary_expression_node(ExpressionNode *left, int op_token_param, ExpressionNode *right, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_BINARY, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_BINARY;
    node->data.binary_expr.left = left;
    node->data.binary_expr.op_token = op_token_param; // CORRECTED
    node->data.binary_expr.right = right;
    node->resolved_type = TYPE_UNDEFINED; // Initialize
    return node;
}

ExpressionNode *create_unary_expression_node(int op_token_param, ExpressionNode *operand, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_UNARY, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_UNARY;
    node->data.unary_expr.op_token = op_token_param; // CORRECTED
    node->data.unary_expr.operand = operand;
    node->resolved_type = TYPE_UNDEFINED; // Initialize
    return node;
}

ExpressionNode *create_literal_expression_node(LiteralNode *literal, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_LITERAL, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_LITERAL;
    node->data.literal_expr = literal;
    node->resolved_type = TYPE_UNDEFINED; // Initialize
    return node;
}

ExpressionNode *create_identifier_expression_node(IdentifierNode *identifier, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_IDENTIFIER, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_IDENTIFIER;
    node->data.identifier_expr = identifier;
    node->resolved_type = TYPE_UNDEFINED; // Initialize
    return node;
}

ExpressionNode *create_array_access_expression_node(IdentifierNode *array_name_ident_param, ExpressionNode *index_expr_param, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_ARRAY_ACCESS, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_ARRAY_ACCESS;
    node->data.array_access_expr.array_name_ident = array_name_ident_param; // CORRECTED
    node->data.array_access_expr.index_expr = index_expr_param;             // CORRECTED
    node->resolved_type = TYPE_UNDEFINED;                                   // Initialize
    return node;
}

ExpressionNode *create_parenthesized_expression_node(ExpressionNode *inner_expr, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_PARENTHESIZED, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_PARENTHESIZED;
    node->data.parenthesized_expr_val = inner_expr; // CORRECTED
    node->resolved_type = TYPE_UNDEFINED;           // Initialize
    return node;
}

// Name from ast.h
ExpressionNode *create_boolean_literal_expression_node(int is_true_param, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_BOOLEAN, sizeof(ExpressionNode), line_number);
    node->kind = is_true_param ? EXPR_BOOLEAN_TRUE : EXPR_BOOLEAN_FALSE; // CORRECTED
    node->resolved_type = TYPE_BOOLEAN;                                  // Initialize
    return node;
}

// --- Other Node Creation Functions ---
IdentifierNode *create_identifier_node(char *name, int line_number)
{
    IdentifierNode *node = (IdentifierNode *)create_node(NODE_IDENTIFIER, sizeof(IdentifierNode), line_number);
    if (name)
    {
        node->name = strdup(name); // Make a copy
        if (!node->name)
        {
            perror("Failed to strdup identifier name");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        node->name = NULL; // Should not happen if lexer provides name
    }
    return node;
}

LiteralNode *create_int_literal_node(int value, int line_number)
{
    LiteralNode *node = (LiteralNode *)create_node(NODE_LITERAL_INTEGER, sizeof(LiteralNode), line_number);
    node->kind = LIT_INTEGER;
    node->data.int_val = value;
    return node;
}

LiteralNode *create_float_literal_node(float value, int line_number)
{
    LiteralNode *node = (LiteralNode *)create_node(NODE_LITERAL_FLOAT, sizeof(LiteralNode), line_number);
    node->kind = LIT_FLOAT;
    node->data.float_val = value;
    return node;
}

LiteralNode *create_char_literal_node(char value, int line_number)
{
    LiteralNode *node = (LiteralNode *)create_node(NODE_LITERAL_CHAR, sizeof(LiteralNode), line_number);
    node->kind = LIT_CHAR;
    node->data.char_val = value;
    return node;
}

LiteralNode *create_string_literal_node(char *value, int line_number)
{
    LiteralNode *node = (LiteralNode *)create_node(NODE_LITERAL_STRING, sizeof(LiteralNode), line_number);
    node->kind = LIT_STRING;
    if (value)
    {
        node->data.string_val = strdup(value);
        if (!node->data.string_val)
        {
            perror("Failed to strdup string literal");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        node->data.string_val = NULL; // Or an empty string ""
    }
    return node;
}

TypeNode *create_type_node(TypeKind type_kind_param, int line_number)
{
    // All TypeNode instances have the base node_type of NODE_TYPE
    TypeNode *node = (TypeNode *)create_node(NODE_TYPE, sizeof(TypeNode), line_number); // CORRECTED
    node->kind = type_kind_param;                                                       // Store the specific type (TYPE_INT, TYPE_CHAR, etc.)
    // The switch statement setting node->base.node_type differently is removed.
    return node;
}

// --- Print Functions (Ensure prototypes in ast.h match these definitions) ---
// Forward declarations for print functions called before definition in this file
void print_class_declaration(ClassDeclarationNode *node, int indent);
void print_identifier(IdentifierNode *node, int indent);
void print_variable_declaration_list(VariableDeclarationListNode *list, int indent);
void print_main_method(MainMethodNode *node, int indent);
void print_block_statement_node(BlockStatementNode *node, int indent);
void print_statement(StatementNode *node, int indent);
void print_variable_declaration_node(VariableDeclarationNode *node, int indent);
void print_type(TypeNode *node, int indent);
void print_expression(ExpressionNode *node, int indent);
void print_assignment_node(AssignmentNode *node, int indent);
void print_if_statement_node(IfStatementNode *node, int indent);
void print_while_statement_node(WhileStatementNode *node, int indent);
void print_print_statement_node(PrintStatementNode *node, int indent);
void print_return_statement_node(ReturnStatementNode *node, int indent);
void print_literal(LiteralNode *node, int indent);

void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf("  ");
    }
}

void print_program_node(ProgramNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("ProgramNode (Line: %d)\n", node->base.line_number);
    print_class_declaration(node->class_decl, indent + 1);
}

void print_class_declaration(ClassDeclarationNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("ClassDeclarationNode (Line: %d)\n", node->base.line_number);
    print_indent(indent + 1);
    printf("Name: ");
    print_identifier(node->name, 0);
    printf("\n");
    if (node->var_decls)
    {
        print_variable_declaration_list(node->var_decls, indent + 1);
    }
    if (node->main_method)
    {
        print_main_method(node->main_method, indent + 1);
    }
}

void print_main_method(MainMethodNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("MainMethodNode (Line: %d)\n", node->base.line_number);
    print_indent(indent + 1);
    printf("Name: ");
    print_identifier(node->name, 0);
    printf("\n");
    print_block_statement_node(node->body, indent + 1);
}

void print_statements(StatementListNode *list, int indent)
{
    StatementListNode *current = list;
    while (current)
    {
        print_statement(current->statement, indent); // Statements in a list are at same level
        current = current->next;
    }
}

void print_statement(StatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("Statement (Line: %d, Type: %d) - Kind: ", node->base.line_number, node->base.node_type);
    switch (node->kind)
    {
    case STMT_KIND_VAR_DECL: // CORRECTED
        printf("Variable Declaration\n");
        print_variable_declaration_node(node->data.var_decl_data, indent + 1); // CORRECTED
        break;
    case STMT_KIND_ASSIGNMENT: // CORRECTED
        printf("Assignment\n");
        print_assignment_node(node->data.assignment_data, indent + 1); // CORRECTED
        break;
    case STMT_KIND_IF: // CORRECTED
        printf("If Statement\n");
        print_if_statement_node(node->data.if_data, indent + 1); // CORRECTED
        break;
    case STMT_KIND_WHILE: // CORRECTED
        printf("While Statement\n");
        print_while_statement_node(node->data.while_data, indent + 1); // CORRECTED
        break;
    case STMT_KIND_PRINT: // CORRECTED
        printf("Print Statement\n");
        print_print_statement_node(node->data.print_data, indent + 1); // CORRECTED
        break;
    case STMT_KIND_RETURN: // CORRECTED
        printf("Return Statement\n");
        print_return_statement_node(node->data.return_data, indent + 1); // CORRECTED
        break;
    case STMT_KIND_BLOCK: // CORRECTED
        printf("Block Statement\n");
        print_block_statement_node(node->data.block_data, indent + 1); // CORRECTED
        break;
    default:
        printf("Unknown Statement Kind (%d)\n", node->kind);
        break;
    }
}

void print_block_statement_node(BlockStatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("BlockStatement (Line: %d)\n", node->base.line_number);
    print_statements(node->statements, indent + 1);
}

void print_variable_declaration_node(VariableDeclarationNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("VariableDeclaration (Line: %d) - %s ", node->base.line_number, node->is_final ? "final" : "");
    print_type(node->var_type, 0);
    printf(" ");
    print_identifier(node->identifier, 0);
    if (node->initializer)
    {
        printf(" = ");
        print_expression(node->initializer, 0);
    }
    printf("\n");
}

void print_variable_declaration_list(VariableDeclarationListNode *list, int indent)
{
    if (!list)
        return;
    print_indent(indent);
    printf("Variable Declarations List:\n");
    VariableDeclarationListNode *current = list;
    while (current)
    {
        print_variable_declaration_node(current->declaration, indent + 1);
        current = current->next;
    }
}

void print_assignment_node(AssignmentNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("Assignment (Line: %d): ", node->base.line_number);
    print_identifier(node->target_identifier, 0);
    if (node->array_index)
    {
        printf("[");
        print_expression(node->array_index, 0);
        printf("]");
    }
    printf(" = ");
    print_expression(node->value, 0);
    printf("\n");
}

void print_if_statement_node(IfStatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("If (Line: %d)\n", node->base.line_number);
    print_indent(indent + 1);
    printf("Condition: ");
    print_expression(node->condition, 0);
    printf("\n");
    print_indent(indent + 1);
    printf("Then Branch:\n");
    print_statement(node->then_branch, indent + 2);
    if (node->else_branch)
    {
        print_indent(indent + 1);
        printf("Else Branch:\n");
        print_statement(node->else_branch, indent + 2);
    }
}

void print_while_statement_node(WhileStatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("While (Line: %d)\n", node->base.line_number);
    print_indent(indent + 1);
    printf("Condition: ");
    print_expression(node->condition, 0);
    printf("\n");
    print_indent(indent + 1);
    printf("Body:\n");
    print_statement(node->body, indent + 2);
}

void print_print_statement_node(PrintStatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("Print (Line: %d): ", node->base.line_number);
    print_expression(node->expression, 0);
    printf("\n");
}

void print_return_statement_node(ReturnStatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("Return (Line: %d): ", node->base.line_number);
    if (node->expression)
    {
        print_expression(node->expression, 0);
    }
    else
    {
        printf("<void>");
    }
    printf("\n");
}

void print_expression(ExpressionNode *node, int indent_level)
{ // Changed param name to avoid conflict
    if (!node)
        return;
    switch (node->kind)
    {
    case EXPR_BINARY:
        printf("(");
        print_expression(node->data.binary_expr.left, 0);
        printf(" %s ", get_token_name_from_int(node->data.binary_expr.op_token)); // CORRECTED
        print_expression(node->data.binary_expr.right, 0);
        printf(")");
        break;
    case EXPR_UNARY:
        printf("%s(", get_token_name_from_int(node->data.unary_expr.op_token)); // CORRECTED
        print_expression(node->data.unary_expr.operand, 0);
        printf(")");
        break;
    case EXPR_LITERAL:
        print_literal(node->data.literal_expr, 0);
        break;
    case EXPR_IDENTIFIER:
        print_identifier(node->data.identifier_expr, 0);
        break;
    case EXPR_ARRAY_ACCESS:
        print_identifier(node->data.array_access_expr.array_name_ident, 0); // CORRECTED
        printf("[");
        print_expression(node->data.array_access_expr.index_expr, 0);
        printf("]"); // CORRECTED
        break;
    case EXPR_PARENTHESIZED:
        printf("(");
        print_expression(node->data.parenthesized_expr_val, 0);
        printf(")"); // CORRECTED
        break;
    case EXPR_BOOLEAN_TRUE: // CORRECTED
        printf("true");
        break;
    case EXPR_BOOLEAN_FALSE: // CORRECTED
        printf("false");
        break;
    default:
        printf("<Unknown Expression Kind: %d>", node->kind);
        break;
    }
}

void print_identifier(IdentifierNode *node, int indent_level)
{
    if (!node || !node->name)
        return;
    printf("%s", node->name);
}

void print_literal(LiteralNode *node, int indent_level)
{
    if (!node)
        return;
    switch (node->kind)
    {
    case LIT_INTEGER:
        printf("%d", node->data.int_val);
        break;
    case LIT_FLOAT:
        printf("%f", node->data.float_val);
        break;
    case LIT_CHAR:
        printf("'%c'", node->data.char_val);
        break;
    case LIT_STRING:
        printf("\"%s\"", node->data.string_val ? node->data.string_val : "");
        break;
    default:
        printf("<Unknown Literal>");
        break;
    }
}

void print_type(TypeNode *node, int indent_level)
{
    if (!node)
        return;
    switch (node->kind)
    { // TypeKind from symbol_table_types.h
    case TYPE_INT:
        printf("int");
        break;
    case TYPE_CHAR:
        printf("char");
        break;
    case TYPE_BOOLEAN:
        printf("boolean");
        break;
    case TYPE_INT_ARRAY:
        printf("int[]");
        break;
    case TYPE_VOID:
        printf("void");
        break;
    case TYPE_FLOAT:
        printf("float");
        break; // Assuming you have this
    case TYPE_STRING:
        printf("String");
        break; // Assuming you have this
    default:
        printf("<Unknown TypeKind: %d>", node->kind);
        break;
    }
}

void print_ast(ProgramNode *node, int indent)
{
    print_program_node(node, indent);
}

// --- Memory Management Functions (Ensure prototypes in ast.h match these definitions) ---
// Forward declarations for free functions called before definition
void free_class_declaration_node(ClassDeclarationNode *node);
void free_identifier_node(IdentifierNode *node);
void free_main_method_node(MainMethodNode *node);
void free_variable_declaration_list_node(VariableDeclarationListNode *node);
void free_block_statement_node(BlockStatementNode *node);
void free_statement_node(StatementNode *node);
void free_variable_declaration_node(VariableDeclarationNode *node);
void free_type_node(TypeNode *node);
void free_expression_node(ExpressionNode *node);
void free_assignment_node(AssignmentNode *node);
void free_if_statement_node(IfStatementNode *node);
void free_while_statement_node(WhileStatementNode *node);
void free_print_statement_node(PrintStatementNode *node);
void free_return_statement_node(ReturnStatementNode *node);
void free_statement_list_node(StatementListNode *node);
void free_literal_node(LiteralNode *node);

void free_ast(ProgramNode *node)
{
    if (!node)
        return;
    free_class_declaration_node(node->class_decl);
    free(node);
}

void free_class_declaration_node(ClassDeclarationNode *node)
{
    if (!node)
        return;
    free_identifier_node(node->name);
    free_main_method_node(node->main_method);
    free_variable_declaration_list_node(node->var_decls);
    // free(node->associated_scope); // If Scope is managed here
    free(node);
}

void free_main_method_node(MainMethodNode *node)
{
    if (!node)
        return;
    free_identifier_node(node->name);
    free_block_statement_node(node->body);
    // free(node->associated_scope); // If Scope is managed here
    free(node);
}

void free_statement_list_node(StatementListNode *node)
{
    StatementListNode *current = node;
    while (current)
    {
        StatementListNode *next = current->next;
        free_statement_node(current->statement);
        free(current);
        current = next;
    }
}

void free_statement_node(StatementNode *node)
{
    if (!node)
        return;
    switch (node->kind)
    {
    case STMT_KIND_VAR_DECL:
        free_variable_declaration_node(node->data.var_decl_data);
        break; // C
    case STMT_KIND_ASSIGNMENT:
        free_assignment_node(node->data.assignment_data);
        break; // C
    case STMT_KIND_IF:
        free_if_statement_node(node->data.if_data);
        break; // C
    case STMT_KIND_WHILE:
        free_while_statement_node(node->data.while_data);
        break; // C
    case STMT_KIND_PRINT:
        free_print_statement_node(node->data.print_data);
        break; // C
    case STMT_KIND_RETURN:
        free_return_statement_node(node->data.return_data);
        break; // C
    case STMT_KIND_BLOCK:
        free_block_statement_node(node->data.block_data);
        break; // C
    case STMT_KIND_EXPRESSION:
        free_expression_node(node->data.expression_data);
        break;
    default:
        break;
    }
    free(node);
}

void free_variable_declaration_node(VariableDeclarationNode *node)
{
    if (!node)
        return;
    free_type_node(node->var_type);
    free_identifier_node(node->identifier);
    free_expression_node(node->initializer);
    free(node);
}

void free_variable_declaration_list_node(VariableDeclarationListNode *node)
{
    VariableDeclarationListNode *current = node;
    while (current)
    {
        VariableDeclarationListNode *next = current->next;
        free_variable_declaration_node(current->declaration);
        free(current);
        current = next;
    }
}

void free_assignment_node(AssignmentNode *node)
{
    if (!node)
        return;
    free_identifier_node(node->target_identifier);
    free_expression_node(node->array_index);
    free_expression_node(node->value);
    free(node);
}

void free_if_statement_node(IfStatementNode *node)
{
    if (!node)
        return;
    free_expression_node(node->condition);
    free_statement_node(node->then_branch);
    free_statement_node(node->else_branch);
    free(node);
}

void free_while_statement_node(WhileStatementNode *node)
{
    if (!node)
        return;
    free_expression_node(node->condition);
    free_statement_node(node->body);
    free(node);
}

void free_print_statement_node(PrintStatementNode *node)
{
    if (!node)
        return;
    free_expression_node(node->expression);
    free(node);
}

void free_return_statement_node(ReturnStatementNode *node)
{
    if (!node)
        return;
    free_expression_node(node->expression);
    free(node);
}

void free_block_statement_node(BlockStatementNode *node)
{
    if (!node)
        return;
    free_statement_list_node(node->statements);
    // free(node->associated_scope); // If Scope is managed here
    free(node);
}

void free_expression_node(ExpressionNode *node)
{
    if (!node)
        return;
    switch (node->kind)
    {
    case EXPR_BINARY:
        free_expression_node(node->data.binary_expr.left);
        free_expression_node(node->data.binary_expr.right);
        break;
    case EXPR_UNARY:
        free_expression_node(node->data.unary_expr.operand);
        break;
    case EXPR_LITERAL:
        free_literal_node(node->data.literal_expr);
        break;
    case EXPR_IDENTIFIER:
        free_identifier_node(node->data.identifier_expr);
        break;
    case EXPR_ARRAY_ACCESS:
        free_identifier_node(node->data.array_access_expr.array_name_ident); // C
        free_expression_node(node->data.array_access_expr.index_expr);       // C
        break;
    case EXPR_PARENTHESIZED:
        free_expression_node(node->data.parenthesized_expr_val); // C
        break;
    case EXPR_BOOLEAN_TRUE:  // C
    case EXPR_BOOLEAN_FALSE: // C
        break;
    default:
        break;
    }
    free(node);
}

void free_identifier_node(IdentifierNode *node)
{
    if (!node)
        return;
    if (node->name)
        free(node->name);
    free(node);
}

void free_literal_node(LiteralNode *node)
{
    if (!node)
        return;
    if (node->kind == LIT_STRING && node->data.string_val)
    {
        free(node->data.string_val);
    }
    free(node);
}

void free_type_node(TypeNode *node)
{
    if (!node)
        return;
    free(node);
}

void free_parsed_class_contents(ParsedClassContents *pcc)
{
    if (!pcc)
        return;
    // Do NOT free pcc->main_method or pcc->var_decls here if they have been
    // transferred to a ClassDeclarationNode. This function is for cleanup
    // if parsing fails *before* transfer.
    // If they are transferred, the ClassDeclarationNode's free function handles them.
    // If this is called on error *before* transfer:
    // free_main_method_node(pcc->main_method);
    // free_variable_declaration_list_node(pcc->var_decls);
    free(pcc);
}