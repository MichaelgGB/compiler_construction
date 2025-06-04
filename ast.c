#include "ast.h"
#include "miniJava.tab.h" // Include Bison-generated token definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declare yytname as extern, provided by miniJava.tab.c when %token-table is used
extern const char *const yytname[];
extern const char *yysymbol_name(int);

const char *get_token_name(int token)
{
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
    default:
        return "UNKNOWN";
    }
}

// --- Helper for creating base AST node ---
AstNode *create_node(AstNodeType type, size_t size, int line_number)
{
    AstNode *node = (AstNode *)calloc(1, size);
    if (!node)
    {
        perror("Failed to allocate AST node");
        exit(EXIT_FAILURE);
    }
    node->type = type;
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
    return node;
}

MainMethodNode *create_main_method_node(IdentifierNode *name, BlockStatementNode *body, int line_number)
{
    MainMethodNode *node = (MainMethodNode *)create_node(NODE_MAIN_METHOD, sizeof(MainMethodNode), line_number);
    node->name = name;
    node->body = body;
    return node;
}

StatementListNode *create_statement_list_node(StatementNode *statement, StatementListNode *next)
{
    StatementListNode *node = (StatementListNode *)create_node(NODE_STATEMENT_LIST, sizeof(StatementListNode), statement ? statement->base.line_number : (next ? next->base.line_number : 0));
    node->statement = statement;
    node->next = next;
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

VariableDeclarationListNode *create_variable_declaration_list_node(VariableDeclarationNode *declaration, VariableDeclarationListNode *next)
{
    VariableDeclarationListNode *node = (VariableDeclarationListNode *)create_node(NODE_VARIABLE_DECLARATION_LIST, sizeof(VariableDeclarationListNode), declaration ? declaration->base.line_number : (next ? next->base.line_number : 0));
    node->declaration = declaration;
    node->next = next;
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
    return node;
}

// --- Statement Wrappers ---
StatementNode *create_variable_declaration_statement(VariableDeclarationNode *var_decl, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_VAR_DECL, sizeof(StatementNode), line_number);
    node->kind = STMT_VAR_DECL;
    node->data.var_decl_stmt = var_decl;
    return node;
}

StatementNode *create_assignment_statement(AssignmentNode *assignment, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_ASSIGNMENT, sizeof(StatementNode), line_number);
    node->kind = STMT_ASSIGNMENT;
    node->data.assignment_stmt = assignment;
    return node;
}

StatementNode *create_if_statement_node(IfStatementNode *if_stmt, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_IF, sizeof(StatementNode), line_number);
    node->kind = STMT_IF;
    node->data.if_stmt = if_stmt;
    return node;
}

StatementNode *create_while_statement_node(WhileStatementNode *while_stmt, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_WHILE, sizeof(StatementNode), line_number);
    node->kind = STMT_WHILE;
    node->data.while_stmt = while_stmt;
    return node;
}

StatementNode *create_print_statement_node(PrintStatementNode *print_stmt, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_PRINT, sizeof(StatementNode), line_number);
    node->kind = STMT_PRINT;
    node->data.print_stmt = print_stmt;
    return node;
}

StatementNode *create_return_statement_node(ReturnStatementNode *return_stmt, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_RETURN, sizeof(StatementNode), line_number);
    node->kind = STMT_RETURN;
    node->data.return_stmt = return_stmt;
    return node;
}

StatementNode *create_block_statement_node(BlockStatementNode *block_stmt, int line_number)
{
    StatementNode *node = (StatementNode *)create_node(NODE_STATEMENT_BLOCK, sizeof(StatementNode), line_number);
    node->kind = STMT_BLOCK;
    node->data.block_stmt = block_stmt;
    return node;
}

// --- Expression Functions ---
ExpressionNode *create_binary_expression_node(ExpressionNode *left, int op, ExpressionNode *right, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_BINARY, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_BINARY;
    node->data.binary_expr.left = left;
    node->data.binary_expr.op = op;
    node->data.binary_expr.right = right;
    return node;
}

ExpressionNode *create_unary_expression_node(int op, ExpressionNode *operand, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_UNARY, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_UNARY;
    node->data.unary_expr.op = op;
    node->data.unary_expr.operand = operand;
    return node;
}

ExpressionNode *create_literal_expression_node(LiteralNode *literal, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_LITERAL, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_LITERAL;
    node->data.literal_expr = literal;
    return node;
}

ExpressionNode *create_identifier_expression_node(IdentifierNode *identifier, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_IDENTIFIER, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_IDENTIFIER;
    node->data.identifier_expr = identifier;
    return node;
}

ExpressionNode *create_array_access_expression_node(IdentifierNode *array_base, ExpressionNode *index, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_ARRAY_ACCESS, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_ARRAY_ACCESS;
    node->data.array_access_expr.array_base = array_base;
    node->data.array_access_expr.index = index;
    return node;
}

ExpressionNode *create_parenthesized_expression_node(ExpressionNode *inner_expr, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_PARENTHESIZED, sizeof(ExpressionNode), line_number);
    node->kind = EXPR_PARENTHESIZED;
    node->data.parenthesized_expr = inner_expr;
    return node;
}

ExpressionNode *create_boolean_expression_node(int token_type, int line_number)
{
    ExpressionNode *node = (ExpressionNode *)create_node(NODE_EXPRESSION_BOOLEAN, sizeof(ExpressionNode), line_number);
    node->kind = (token_type == TOKEN_TRUE) ? EXPR_TRUE : EXPR_FALSE;
    return node;
}

// --- Other Node Creation Functions ---
IdentifierNode *create_identifier_node(char *name, int line_number)
{
    IdentifierNode *node = (IdentifierNode *)create_node(NODE_IDENTIFIER, sizeof(IdentifierNode), line_number);
    node->name = strdup(name); // Make a copy
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
    node->data.string_val = strdup(value);
    return node;
}

TypeNode *create_type_node(TypeKind kind, int line_number)
{
    TypeNode *node = (TypeNode *)create_node(NODE_TYPE_INT, sizeof(TypeNode), line_number); // Changed to a generic type node base
    node->kind = kind;
    // Set specific base type based on kind
    switch (kind)
    {
    case TYPE_INT:
        node->base.type = NODE_TYPE_INT;
        break;
    case TYPE_CHAR:
        node->base.type = NODE_TYPE_CHAR;
        break;
    case TYPE_BOOLEAN:
        node->base.type = NODE_TYPE_BOOLEAN;
        break;
    case TYPE_INT_ARRAY:
        node->base.type = NODE_TYPE_INT_ARRAY;
        break;
    }
    return node;
}

// --- Print Functions ---
void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf("  "); // 2 spaces per indent level
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
    print_identifier(node->name, indent + 1);
    printf("\n");
    if (node->var_decls)
    {
        print_variable_declaration_list(node->var_decls, indent + 1);
    }
    print_main_method(node->main_method, indent + 1);
}

void print_main_method(MainMethodNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("MainMethodNode (Line: %d)\n", node->base.line_number);
    print_indent(indent + 1);
    printf("Name: ");
    print_identifier(node->name, indent + 1);
    printf("\n");
    print_block_statement_node(node->body, indent + 1);
}

void print_statements(StatementListNode *list, int indent)
{
    if (!list)
        return;
    print_indent(indent);
    printf("Statements:\n");
    StatementListNode *current = list;
    while (current)
    {
        print_statement(current->statement, indent + 1);
        current = current->next;
    }
}

void print_statement(StatementNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("Statement (Line: %d) - Kind: ", node->base.line_number);
    switch (node->kind)
    {
    case STMT_VAR_DECL:
        printf("Variable Declaration\n");
        print_variable_declaration_node(node->data.var_decl_stmt, indent + 1);
        break;
    case STMT_ASSIGNMENT:
        printf("Assignment\n");
        print_assignment_node(node->data.assignment_stmt, indent + 1);
        break;
    case STMT_IF:
        printf("If Statement\n");
        print_if_statement_node(node->data.if_stmt, indent + 1);
        break;
    case STMT_WHILE:
        printf("While Statement\n");
        print_while_statement_node(node->data.while_stmt, indent + 1);
        break;
    case STMT_PRINT:
        printf("Print Statement\n");
        print_print_statement_node(node->data.print_stmt, indent + 1);
        break;
    case STMT_RETURN:
        printf("Return Statement\n");
        print_return_statement_node(node->data.return_stmt, indent + 1);
        break;
    case STMT_BLOCK:
        printf("Block Statement\n");
        print_block_statement_node(node->data.block_stmt, indent + 1);
        break;
    default:
        printf("Unknown Statement Kind\n");
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
    print_type(node->var_type, indent); // Adjusted indent for inline
    printf(" ");
    print_identifier(node->identifier, indent); // Adjusted indent for inline
    if (node->initializer)
    {
        printf(" = ");
        print_expression(node->initializer, indent); // Adjusted indent for inline
    }
    printf("\n");
}

void print_variable_declaration_list(VariableDeclarationListNode *list, int indent)
{
    if (!list)
        return;
    print_indent(indent);
    printf("Variable Declarations:\n");
    VariableDeclarationListNode *current = list;
    while (current)
    {
        print_indent(indent + 1);
        printf("- VarDecl: ");
        print_type(current->declaration->var_type, indent + 1);
        printf(" ");
        print_identifier(current->declaration->identifier, indent + 1);
        if (current->declaration->initializer)
        {
            printf(" = ");
            print_expression(current->declaration->initializer, indent + 1);
        }
        printf("\n");
        current = current->next;
    }
}

void print_assignment_node(AssignmentNode *node, int indent)
{
    if (!node)
        return;
    print_indent(indent);
    printf("Assignment (Line: %d): ", node->base.line_number);
    print_identifier(node->target_identifier, indent);
    if (node->array_index)
    {
        printf("[");
        print_expression(node->array_index, indent);
        printf("]");
    }
    printf(" = ");
    print_expression(node->value, indent);
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
    print_expression(node->condition, indent + 1);
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
    print_expression(node->condition, indent + 1);
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
    print_expression(node->expression, indent);
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
        print_expression(node->expression, indent);
    }
    else
    {
        printf("<void>");
    }
    printf("\n");
}

void print_expression(ExpressionNode *node, int indent)
{
    if (!node)
        return;
    // Expressions are usually printed inline, so no new line/indentation at start
    switch (node->kind)
    {
    case EXPR_BINARY:
        printf("(");
        print_expression(node->data.binary_expr.left, indent);
        printf("Token: %s\n", get_token_name(node->data.binary_expr.op));
        print_expression(node->data.binary_expr.right, indent);
        printf(")");
        break;
    case EXPR_UNARY:
        printf("%s", get_token_name(node->data.unary_expr.op));
        print_expression(node->data.unary_expr.operand, indent);
        break;
    case EXPR_LITERAL:
        print_literal(node->data.literal_expr, indent);
        break;
    case EXPR_IDENTIFIER:
        print_identifier(node->data.identifier_expr, indent);
        break;
    case EXPR_ARRAY_ACCESS:
        print_identifier(node->data.array_access_expr.array_base, indent);
        printf("[");
        print_expression(node->data.array_access_expr.index, indent);
        printf("]");
        break;
    case EXPR_PARENTHESIZED:
        printf("(");
        print_expression(node->data.parenthesized_expr, indent);
        printf(")");
        break;
    case EXPR_TRUE:
        printf("true");
        break;
    case EXPR_FALSE:
        printf("false");
        break;
    default:
        printf("<Unknown Expression>");
        break;
    }
}

void print_identifier(IdentifierNode *node, int indent)
{
    if (!node)
        return;
    printf("%s", node->name);
}

void print_literal(LiteralNode *node, int indent)
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
        printf("\"%s\"", node->data.string_val);
        break;
    default:
        printf("<Unknown Literal>");
        break;
    }
}

void print_type(TypeNode *node, int indent)
{
    if (!node)
        return;
    switch (node->kind)
    {
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
    default:
        printf("<Unknown Type>");
        break;
    }
}

void print_ast(ProgramNode *node, int indent)
{
    print_program_node(node, indent);
}

// --- Memory Management Functions ---
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
    free(node);
}

void free_main_method_node(MainMethodNode *node)
{
    if (!node)
        return;
    free_identifier_node(node->name);
    free_block_statement_node(node->body);
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
    case STMT_VAR_DECL:
        free_variable_declaration_node(node->data.var_decl_stmt);
        break;
    case STMT_ASSIGNMENT:
        free_assignment_node(node->data.assignment_stmt);
        break;
    case STMT_IF:
        free_if_statement_node(node->data.if_stmt);
        break;
    case STMT_WHILE:
        free_while_statement_node(node->data.while_stmt);
        break;
    case STMT_PRINT:
        free_print_statement_node(node->data.print_stmt);
        break;
    case STMT_RETURN:
        free_return_statement_node(node->data.return_stmt);
        break;
    case STMT_BLOCK:
        free_block_statement_node(node->data.block_stmt);
        break;
    default:
        break; // Should not happen
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
        free_identifier_node(node->data.array_access_expr.array_base);
        free_expression_node(node->data.array_access_expr.index);
        break;
    case EXPR_PARENTHESIZED:
        free_expression_node(node->data.parenthesized_expr);
        break;
    case EXPR_TRUE:
    case EXPR_FALSE:
        // No dynamic memory to free for these literals
        break;
    default:
        break; // Should not happen
    }
    free(node);
}

void free_identifier_node(IdentifierNode *node)
{
    if (!node)
        return;
    free(node->name); // Free the strdup'd string
    free(node);
}

void free_literal_node(LiteralNode *node)
{
    if (!node)
        return;
    if (node->kind == LIT_STRING)
    {
        free(node->data.string_val); // Free the strdup'd string for string literals
    }
    free(node);
}

void free_type_node(TypeNode *node)
{
    if (!node)
        return;
    free(node);
}