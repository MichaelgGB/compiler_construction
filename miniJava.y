%{
// Topmost C block: Keep only very basic includes or forward declarations
// that do not depend on ast.h types yet.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Externs for Flex variables that Bison needs to know about.
// yylex() is declared later. yyerror() is defined at the bottom.
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
%}

%code requires {
    // This code is inserted near the top of the generated C file,
    // before YYSTYPE (the union) is defined. Ideal for #includes that define
    // types used in the %union.
    #include "ast.h" // Must define all AST node types, ProgramNode, and ParsedClassContents
}

// The %union block defines the types that can be associated with grammar symbols.
// struct ProgramNode, etc., are known here because ast.h was included via %code requires.
%union {
    int int_val;
    float float_val;
    char char_val;
    char *string_val; // Assumed to be dynamically allocated (e.g., strdup) by lexer

    // AST Node Pointers
    struct ProgramNode *program_node;
    struct ClassDeclarationNode *class_decl_node;
    struct MainMethodNode *main_method_node;
    struct StatementNode *statement_node;           // Generic statement
    struct StatementListNode *statement_list_node;
    struct VariableDeclarationNode *var_decl_node;
    struct VariableDeclarationListNode *var_decl_list_node;
    struct AssignmentNode *assignment_node;
    struct IfStatementNode *if_stmt_node;           // Specific if node (data part)
    struct WhileStatementNode *while_stmt_node;     // Specific while node (data part)
    struct PrintStatementNode *print_stmt_node;     // Specific print node (data part)
    struct ReturnStatementNode *return_stmt_node;   // Specific return node (data part)
    struct BlockStatementNode *block_stmt_node;     // Specific block node (data part)
    struct ExpressionNode *expression_node;
    struct IdentifierNode *identifier_node;
    struct LiteralNode *literal_node;
    struct TypeNode *type_node;

    // Temporary structure for parsing class body
    ParsedClassContents *parsed_class_contents_ptr;
}

// This C block is inserted by Bison *after* %code requires and *after* %union.
// At this point, ast.h has been included, and YYSTYPE is defined.
// ProgramNode is a known type here.
%{
    ProgramNode *root_ast_node = NULL; // Your global AST root - DEFINITION MOVED HERE

    // Forward declare yylex() which is provided by Flex.
    // yyerror() is defined at the bottom of this file.
    extern int yylex(void);
    void yyerror(const char *s);
%}

// TOKEN DEFINITIONS
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

// NON-TERMINAL TYPE ASSOCIATIONS
%type <program_node> program
%type <class_decl_node> class_declaration
%type <main_method_node> main_method_declaration
%type <parsed_class_contents_ptr> class_body_items

%type <statement_list_node> statements
%type <statement_node> statement

%type <var_decl_node> variable_declaration
%type <assignment_node> assignment_core
%type <if_stmt_node> if_statement_core
%type <while_stmt_node> while_statement_core
%type <print_stmt_node> print_statement_core
%type <return_stmt_node> return_statement_core
%type <block_stmt_node> block_statement_core

%type <expression_node> expression logical_or_expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression primary_expression
%type <identifier_node> identifier
%type <literal_node> integer_literal float_literal char_literal string_literal
%type <type_node> type

// Precedence and Associativity
%right TOKEN_ASSIGN
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left TOKEN_LT TOKEN_GT TOKEN_LEQ TOKEN_GEQ
%left TOKEN_PLUS TOKEN_MINUS
%left TOKEN_MULTIPLY TOKEN_DIVIDE TOKEN_MOD
%right TOKEN_NOT
%nonassoc TOKEN_IF_WITHOUT_ELSE
%nonassoc TOKEN_ELSE

%start program

%%

program:
    class_declaration
    {
        $$ = create_program_node($1, @1.first_line);
        root_ast_node = $$; // This assignment should now be type-correct
    }
;

class_body_items:
    /* empty */
    {
        $$ = (ParsedClassContents*)calloc(1, sizeof(ParsedClassContents));
        if (!$$) {
            yyerror("Memory allocation failed for class contents collector");
            YYABORT;
        }
        $$->main_method = NULL;
        $$->var_decls = NULL;
    }
|   class_body_items variable_declaration TOKEN_SEMICOLON
    {
        $$ = $1;
        $$->var_decls = create_variable_declaration_list_node($2, $$->var_decls);
        if (!$$->var_decls && $2) {
             yyerror("Memory allocation failed for var_decl list node"); YYABORT;
        }
    }
|   class_body_items main_method_declaration
    {
        $$ = $1;
        if ($$->main_method != NULL) {
            yyerror("Error: Multiple main methods defined. Using the last one encountered.");
            // Consider: free_main_method_node($$->main_method);
        }
        $$->main_method = $2;
    }
;

class_declaration:
    TOKEN_CLASS identifier TOKEN_LBRACE class_body_items TOKEN_RBRACE
    {
        if (!$4->main_method) {
            yyerror("Error: Class must contain a main method.");
            // YYABORT; // Or handle by creating a default/error node
        }
        $$ = create_class_declaration_node(
            $2,
            $4->main_method,
            $4->var_decls,
            @1.first_line
        );
        free($4);
    }
;

main_method_declaration:
    TOKEN_PUBLIC TOKEN_STATIC TOKEN_VOID identifier TOKEN_LPAREN TOKEN_RPAREN block_statement_core
    {
        // Optional: Check if $4->name is "main"
        // if (strcmp($4->name, "main") != 0) {
        //     yyerror("Main method must be named 'main'");
        // }
        $$ = create_main_method_node($4, $7, @1.first_line);
    }
;

statements:
    /* Empty */
    { $$ = NULL; }
|   statement statements
    { $$ = create_statement_list_node($1, $2); }
;

statement:
    variable_declaration TOKEN_SEMICOLON
    { $$ = create_variable_declaration_statement($1, @1.first_line); }
|   assignment_core TOKEN_SEMICOLON
    { $$ = create_assignment_statement($1, @1.first_line); }
|   if_statement_core
    { $$ = create_if_statement_wrapper_node($1, @1.first_line); }
|   while_statement_core
    { $$ = create_while_statement_wrapper_node($1, @1.first_line); }
|   print_statement_core TOKEN_SEMICOLON
    { $$ = create_print_statement_wrapper_node($1, @1.first_line); }
|   return_statement_core TOKEN_SEMICOLON
    { $$ = create_return_statement_wrapper_node($1, @1.first_line); }
|   block_statement_core
    { $$ = create_block_statement_wrapper_node($1, @1.first_line); }
;

block_statement_core:
    TOKEN_LBRACE statements TOKEN_RBRACE
    { $$ = create_block_node($2, @1.first_line); }
;

variable_declaration:
    type identifier
    { $$ = create_variable_declaration_node($1, $2, NULL, 0, @1.first_line); }
|   type identifier TOKEN_ASSIGN expression
    { $$ = create_variable_declaration_node($1, $2, $4, 0, @1.first_line); }
|   TOKEN_FINAL type identifier TOKEN_ASSIGN expression
    { $$ = create_variable_declaration_node($2, $3, $5, 1, @1.first_line); }
;

type:
    TOKEN_INT
    { $$ = create_type_node(TYPE_INT, @1.first_line); }
|   TOKEN_CHAR
    { $$ = create_type_node(TYPE_CHAR, @1.first_line); }
|   TOKEN_BOOLEAN
    { $$ = create_type_node(TYPE_BOOLEAN, @1.first_line); }
|   TOKEN_INT TOKEN_LBRACKET TOKEN_RBRACKET
    { $$ = create_type_node(TYPE_INT_ARRAY, @1.first_line); }
;

assignment_core:
    identifier TOKEN_ASSIGN expression
    { $$ = create_assignment_node($1, NULL, $3, @1.first_line); }
|   identifier TOKEN_LBRACKET expression TOKEN_RBRACKET TOKEN_ASSIGN expression
    { $$ = create_assignment_node($1, $3, $6, @1.first_line); }
;

if_statement_core:
    TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement %prec TOKEN_IF_WITHOUT_ELSE
    { $$ = create_if_node($3, $5, NULL, @1.first_line); }
|   TOKEN_IF TOKEN_LPAREN expression TOKEN_RPAREN statement TOKEN_ELSE statement
    { $$ = create_if_node($3, $5, $7, @1.first_line); }
;

while_statement_core:
    TOKEN_WHILE TOKEN_LPAREN expression TOKEN_RPAREN statement
    { $$ = create_while_node($3, $5, @1.first_line); }
;

print_statement_core:
    TOKEN_PRINT TOKEN_LPAREN expression TOKEN_RPAREN
    { $$ = create_print_node($3, @1.first_line); }
;

return_statement_core:
    TOKEN_RETURN
    { $$ = create_return_node(NULL, @1.first_line); }
|   TOKEN_RETURN expression
    { $$ = create_return_node($2, @1.first_line); }
;

expression:
    logical_or_expression { $$ = $1; }
;

logical_or_expression:
    logical_and_expression { $$ = $1; }
|   logical_or_expression TOKEN_OR logical_and_expression { $$ = create_binary_expression_node($1, TOKEN_OR, $3, @2.first_line); }
;

logical_and_expression:
    equality_expression { $$ = $1; }
|   logical_and_expression TOKEN_AND equality_expression { $$ = create_binary_expression_node($1, TOKEN_AND, $3, @2.first_line); }
;

equality_expression:
    relational_expression { $$ = $1; }
|   equality_expression TOKEN_EQ relational_expression { $$ = create_binary_expression_node($1, TOKEN_EQ, $3, @2.first_line); }
|   equality_expression TOKEN_NEQ relational_expression { $$ = create_binary_expression_node($1, TOKEN_NEQ, $3, @2.first_line); }
;

relational_expression:
    additive_expression { $$ = $1; }
|   relational_expression TOKEN_LT additive_expression { $$ = create_binary_expression_node($1, TOKEN_LT, $3, @2.first_line); }
|   relational_expression TOKEN_GT additive_expression { $$ = create_binary_expression_node($1, TOKEN_GT, $3, @2.first_line); }
|   relational_expression TOKEN_LEQ additive_expression { $$ = create_binary_expression_node($1, TOKEN_LEQ, $3, @2.first_line); }
|   relational_expression TOKEN_GEQ additive_expression { $$ = create_binary_expression_node($1, TOKEN_GEQ, $3, @2.first_line); }
;

additive_expression:
    multiplicative_expression { $$ = $1; }
|   additive_expression TOKEN_PLUS multiplicative_expression { $$ = create_binary_expression_node($1, TOKEN_PLUS, $3, @2.first_line); }
|   additive_expression TOKEN_MINUS multiplicative_expression { $$ = create_binary_expression_node($1, TOKEN_MINUS, $3, @2.first_line); }
;

multiplicative_expression:
    unary_expression { $$ = $1; }
|   multiplicative_expression TOKEN_MULTIPLY unary_expression { $$ = create_binary_expression_node($1, TOKEN_MULTIPLY, $3, @2.first_line); }
|   multiplicative_expression TOKEN_DIVIDE unary_expression { $$ = create_binary_expression_node($1, TOKEN_DIVIDE, $3, @2.first_line); }
|   multiplicative_expression TOKEN_MOD unary_expression { $$ = create_binary_expression_node($1, TOKEN_MOD, $3, @2.first_line); }
;

unary_expression:
    primary_expression { $$ = $1; }
|   TOKEN_MINUS unary_expression
    { $$ = create_unary_expression_node(TOKEN_MINUS, $2, @1.first_line); }
|   TOKEN_NOT unary_expression
    { $$ = create_unary_expression_node(TOKEN_NOT, $2, @1.first_line); }
;

primary_expression:
    integer_literal
    { $$ = create_literal_expression_node($1, @1.first_line); }
|   float_literal
    { $$ = create_literal_expression_node($1, @1.first_line); }
|   char_literal
    { $$ = create_literal_expression_node($1, @1.first_line); }
|   string_literal
    { $$ = create_literal_expression_node($1, @1.first_line); }
|   TOKEN_TRUE
    { $$ = create_boolean_literal_expression_node(1, @1.first_line); }
|   TOKEN_FALSE
    { $$ = create_boolean_literal_expression_node(0, @1.first_line); }
|   identifier
    { $$ = create_identifier_expression_node($1, @1.first_line); }
|   identifier TOKEN_LBRACKET expression TOKEN_RBRACKET
    { $$ = create_array_access_expression_node($1, $3, @1.first_line); }
|   TOKEN_LPAREN expression TOKEN_RPAREN
    { $$ = $2; }
;

identifier:
    TOKEN_ID
    {
        $$ = create_identifier_node($1, @1.first_line);
    }
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
    TOKEN_STRING_LIT
    {
        $$ = create_string_literal_node($1, @1.first_line);
    }
;

%%

// Standard Bison error reporting function
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error on line %d near token '%s': %s\n", yylineno, yytext, s);
}

// The main function is typically in a separate file (e.g., main.c or your driver file)
// to keep the parser file focused on grammar.
/*
int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    } else {
        yyin = stdin; // Default to stdin if no file is provided
    }

    int parse_result = yyparse(); // Call the parser

    if (yyin != stdin) {
        fclose(yyin);
    }

    if (parse_result == 0) { // 0 indicates success
        printf("Parsing successful!\n");
        if (root_ast_node) {
            printf("AST Root created.\n");
            // Here you would typically call functions for semantic analysis,
            // code generation, printing the AST, etc.
            // For example: print_ast(root_ast_node, 0);
            // And finally, free the AST: free_ast(root_ast_node);
        } else {
            printf("Parsing successful, but no AST root was created (grammar issue?).\n");
        }
        return 0;
    } else { // Non-zero indicates failure
        printf("Parsing failed.\n");
        return 1;
    }
}
*/