%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"


ProgramNode *root_ast_node;

void yyerror(const char *s);
extern int yylex();
extern FILE *yyin;
extern int yylineno;

%}

%union {
    int int_val;
    float float_val;
    char char_val;
    char *string_val;
    ProgramNode *program_node;
    ClassDeclarationNode *class_decl_node;
    MainMethodNode *main_method_node;
    StatementNode *statement_node;
    StatementListNode *statement_list_node;
    VariableDeclarationNode *var_decl_node;
    VariableDeclarationListNode *var_decl_list_node;
    AssignmentNode *assignment_node;
    IfStatementNode *if_stmt_node;
    WhileStatementNode *while_stmt_node;
    PrintStatementNode *print_stmt_node;
    ReturnStatementNode *return_stmt_node;
    BlockStatementNode *block_stmt_node;
    ExpressionNode *expression_node;
    IdentifierNode *identifier_node;
    LiteralNode *literal_node;
    TypeNode *type_node;
    ClassBodyContent *class_body_content_ptr;
}

%token <int_val> TOKEN_INT_LIT
%token <float_val> TOKEN_FLOAT_LIT
%token <char_val> TOKEN_CHAR_LIT
%token <string_val> TOKEN_STRING_LIT
%token <string_val> TOKEN_ID

%token TOKEN_CLASS TOKEN_PUBLIC TOKEN_STATIC TOKEN_VOID
%token TOKEN_INT TOKEN_CHAR TOKEN_BOOLEAN TOKEN_IF
%token TOKEN_ELSE TOKEN_WHILE TOKEN_RETURN TOKEN_TRUE
%token TOKEN_FALSE TOKEN_FINAL TOKEN_PRINT

%token TOKEN_PLUS TOKEN_MINUS TOKEN_MULTIPLY TOKEN_DIVIDE
%token TOKEN_MOD TOKEN_EQ TOKEN_NEQ TOKEN_LT TOKEN_GT
%token TOKEN_LEQ TOKEN_GEQ TOKEN_AND TOKEN_OR TOKEN_NOT
%token TOKEN_ASSIGN

%token TOKEN_LPAREN TOKEN_RPAREN TOKEN_LBRACE TOKEN_RBRACE
%token TOKEN_LBRACKET TOKEN_RBRACKET TOKEN_SEMICOLON TOKEN_COMMA

%type <program_node> program
%type <class_decl_node> class_declaration
%type <main_method_node> main_method
%type <statement_list_node> statements
%type <statement_node> statement
%type <block_stmt_node> block_statement
%type <var_decl_node> variable_declaration
%type <assignment_node> assignment
%type <if_stmt_node> if_statement
%type <while_stmt_node> while_statement
%type <print_stmt_node> print_statement
%type <expression_node> expression logical_or_expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression
%type <identifier_node> identifier
%type <class_body_content_ptr> class_body_content
%type <literal_node> integer_literal float_literal char_literal string_literal
%type <type_node> type

%token-table

%nonassoc TOKEN_IF
%nonassoc TOKEN_ELSE

%start program

%%

program:
    class_declaration {
        $$ = create_program_node($1, @1.first_line);
        root_ast_node = $$;
    }
    ;

class_body_content:
    /* Empty */ {
        ClassBodyContent *content = malloc(sizeof(ClassBodyContent));
        if (!content) {
            yyerror("Memory allocation failed");
            exit(1);
        }
        content->main_method = NULL;
        content->var_declarations = NULL;
        $$ = content;
    }
    | class_body_content main_method {
        $1->main_method = $2;
        $$ = $1;
    }
    | class_body_content variable_declaration TOKEN_SEMICOLON {
        VariableDeclarationListNode *new_list_node = create_variable_declaration_list_node($2, $1->var_declarations);
        $1->var_declarations = new_list_node;
        $$ = $1;
    }
    ;

class_declaration:
    TOKEN_CLASS identifier TOKEN_LBRACE class_body_content TOKEN_RBRACE {
        $$ = create_class_declaration_node($2, $4->main_method, $4->var_declarations, @1.first_line);
        free($4);
    }
    ;

main_method:
    TOKEN_PUBLIC TOKEN_STATIC TOKEN_VOID identifier TOKEN_LPAREN TOKEN_RPAREN TOKEN_LBRACE statements TOKEN_RBRACE {
        $$ = create_main_method_node($4, create_block_node($8, @1.first_line), @1.first_line);
    }
    ;

statements:
    /* Empty */ { $$ = NULL; }
    | statement statements { $$ = create_statement_list_node($1, $2); }
    ;

statement:
    variable_declaration TOKEN_SEMICOLON { $$ = create_variable_declaration_statement($1, @1.first_line); }
    | assignment TOKEN_SEMICOLON { $$ = create_assignment_statement($1, @1.first_line); }
    | if_statement { $$ = create_if_statement_node($1, @1.first_line); }
    | while_statement { $$ = create_while_statement_node($1, @1.first_line); }
    | print_statement TOKEN_SEMICOLON { $$ = create_print_statement_node($1, @1.first_line); }
    | TOKEN_RETURN expression TOKEN_SEMICOLON { $$ = create_return_statement_node(create_return_node($2, @1.first_line), @1.first_line); }
    | block_statement { $$ = create_block_statement_node($1, @1.first_line); }
    ;

block_statement:
    TOKEN_LBRACE statements TOKEN_RBRACE { $$ = create_block_node($2, @1.first_line); }
    ;

variable_declaration:
    type identifier { $$ = create_variable_declaration_node($1, $2, NULL, 0, @1.first_line); }
    | type identifier TOKEN_ASSIGN expression { $$ = create_variable_declaration_node($1, $2, $4, 0, @1.first_line); }
    | TOKEN_FINAL type identifier TOKEN_ASSIGN expression { $$ = create_variable_declaration_node($2, $3, $5, 1, @1.first_line); }
    ;

type:
    TOKEN_INT { $$ = create_type_node(TYPE_INT, @1.first_line); }
    | TOKEN_CHAR { $$ = create_type_node(TYPE_CHAR, @1.first_line); }
    | TOKEN_BOOLEAN { $$ = create_type_node(TYPE_BOOLEAN, @1.first_line); }
    | TOKEN_INT TOKEN_LBRACKET TOKEN_RBRACKET { $$ = create_type_node(TYPE_INT_ARRAY, @1.first_line); }
    ;

assignment:
    identifier TOKEN_ASSIGN expression { $$ = create_assignment_node($1, NULL, $3, @1.first_line); }
    | identifier TOKEN_LBRACKET expression TOKEN_RBRACKET TOKEN_ASSIGN expression { $$ = create_assignment_node($1, $3, $6, @1.first_line); }
    ;

if_statement:
    TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement { $$ = create_if_node($3, $5, NULL, @1.first_line); }
    | TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement TOKEN_ELSE statement { $$ = create_if_node($3, $5, $7, @1.first_line); }
    ;

while_statement:
    TOKEN_WHILE TOKEN_LPAREN expression TOKEN_RPAREN statement { $$ = create_while_node($3, $5, @1.first_line); }
    ;

print_statement:
    TOKEN_PRINT TOKEN_LPAREN expression TOKEN_RPAREN { $$ = create_print_node($3, @1.first_line); }
    ;

expression:
    logical_or_expression { $$ = $1; }
    ;

logical_or_expression:
    logical_and_expression { $$ = $1; }
    | logical_or_expression TOKEN_OR logical_and_expression { $$ = create_binary_expression_node($1, TOKEN_OR, $3, @2.first_line); }
    ;

logical_and_expression:
    equality_expression { $$ = $1; }
    | logical_and_expression TOKEN_AND equality_expression { $$ = create_binary_expression_node($1, TOKEN_AND, $3, @2.first_line); }
    ;

equality_expression:
    relational_expression { $$ = $1; }
    | equality_expression TOKEN_EQ relational_expression { $$ = create_binary_expression_node($1, TOKEN_EQ, $3, @2.first_line); }
    | equality_expression TOKEN_NEQ relational_expression { $$ = create_binary_expression_node($1, TOKEN_NEQ, $3, @2.first_line); }
    ;

relational_expression:
    additive_expression { $$ = $1; }
    | relational_expression TOKEN_LT additive_expression { $$ = create_binary_expression_node($1, TOKEN_LT, $3, @2.first_line); }
    | relational_expression TOKEN_GT additive_expression { $$ = create_binary_expression_node($1, TOKEN_GT, $3, @2.first_line); }
    | relational_expression TOKEN_LEQ additive_expression { $$ = create_binary_expression_node($1, TOKEN_LEQ, $3, @2.first_line); }
    | relational_expression TOKEN_GEQ additive_expression { $$ = create_binary_expression_node($1, TOKEN_GEQ, $3, @2.first_line); }
    ;

additive_expression:
    multiplicative_expression { $$ = $1; }
    | additive_expression TOKEN_PLUS multiplicative_expression { $$ = create_binary_expression_node($1, TOKEN_PLUS, $3, @2.first_line); }
    | additive_expression TOKEN_MINUS multiplicative_expression { $$ = create_binary_expression_node($1, TOKEN_MINUS, $3, @2.first_line); }
    ;

multiplicative_expression:
    unary_expression { $$ = $1; }
    | multiplicative_expression TOKEN_MULTIPLY unary_expression { $$ = create_binary_expression_node($1, TOKEN_MULTIPLY, $3, @2.first_line); }
    | multiplicative_expression TOKEN_DIVIDE unary_expression { $$ = create_binary_expression_node($1, TOKEN_DIVIDE, $3, @2.first_line); }
    | multiplicative_expression TOKEN_MOD unary_expression { $$ = create_binary_expression_node($1, TOKEN_MOD, $3, @2.first_line); }
    ;

unary_expression:
    primary_expression { $$ = $1; }
    | TOKEN_MINUS unary_expression { $$ = create_unary_expression_node(TOKEN_MINUS, $2, @1.first_line); }
    | TOKEN_NOT unary_expression { $$ = create_unary_expression_node(TOKEN_NOT, $2, @1.first_line); }
    ;

primary_expression:
    integer_literal { $$ = create_literal_expression_node($1, @1.first_line); }
    | float_literal { $$ = create_literal_expression_node($1, @1.first_line); }
    | char_literal { $$ = create_literal_expression_node($1, @1.first_line); }
    | string_literal { $$ = create_literal_expression_node($1, @1.first_line); }
    | TOKEN_TRUE { $$ = create_boolean_expression_node(TOKEN_TRUE, @1.first_line); }
    | TOKEN_FALSE { $$ = create_boolean_expression_node(TOKEN_FALSE, @1.first_line); }
    | identifier { $$ = create_identifier_expression_node($1, @1.first_line); }
    | identifier TOKEN_LBRACKET expression TOKEN_RBRACKET { $$ = create_array_access_expression_node($1, $3, @1.first_line); }
    | TOKEN_LPAREN expression TOKEN_RPAREN { $$ = create_parenthesized_expression_node($2, @1.first_line); }
    ;

identifier:
    TOKEN_ID { $$ = create_identifier_node(strdup($1), @1.first_line); }
    ;

integer_literal:
    TOKEN_INT_LIT { $$ = create_int_literal_node($1, @1.first_line); }
    ;

float_literal:
    TOKEN_FLOAT_LIT { $$ = create_float_literal_node($1, @1.first_line); }
    ;

char_literal:
    TOKEN_CHAR_LIT { $$ = create_char_literal_node($1, @1.first_line); }
    ;

string_literal:
    TOKEN_STRING_LIT { $$ = create_string_literal_node(strdup($1), @1.first_line); }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
}

