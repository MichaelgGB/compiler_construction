/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_MINIJAVA_TAB_H_INCLUDED
# define YY_YY_MINIJAVA_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 15 "miniJava.y"

    // This code is inserted near the top of the generated C file,
    // before YYSTYPE (the union) is defined. Ideal for #includes that define
    // types used in the %union.
    #include "ast.h" // Must define all AST node types, ProgramNode, and ParsedClassContents

#line 56 "miniJava.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    TOKEN_INT_LIT = 258,           /* TOKEN_INT_LIT  */
    TOKEN_FLOAT_LIT = 259,         /* TOKEN_FLOAT_LIT  */
    TOKEN_CHAR_LIT = 260,          /* TOKEN_CHAR_LIT  */
    TOKEN_STRING_LIT = 261,        /* TOKEN_STRING_LIT  */
    TOKEN_ID = 262,                /* TOKEN_ID  */
    TOKEN_CLASS = 263,             /* TOKEN_CLASS  */
    TOKEN_PUBLIC = 264,            /* TOKEN_PUBLIC  */
    TOKEN_STATIC = 265,            /* TOKEN_STATIC  */
    TOKEN_VOID = 266,              /* TOKEN_VOID  */
    TOKEN_INT = 267,               /* TOKEN_INT  */
    TOKEN_CHAR = 268,              /* TOKEN_CHAR  */
    TOKEN_BOOLEAN = 269,           /* TOKEN_BOOLEAN  */
    TOKEN_IF = 270,                /* TOKEN_IF  */
    TOKEN_ELSE = 271,              /* TOKEN_ELSE  */
    TOKEN_WHILE = 272,             /* TOKEN_WHILE  */
    TOKEN_RETURN = 273,            /* TOKEN_RETURN  */
    TOKEN_TRUE = 274,              /* TOKEN_TRUE  */
    TOKEN_FALSE = 275,             /* TOKEN_FALSE  */
    TOKEN_FINAL = 276,             /* TOKEN_FINAL  */
    TOKEN_PRINT = 277,             /* TOKEN_PRINT  */
    TOKEN_PLUS = 278,              /* TOKEN_PLUS  */
    TOKEN_MINUS = 279,             /* TOKEN_MINUS  */
    TOKEN_MULTIPLY = 280,          /* TOKEN_MULTIPLY  */
    TOKEN_DIVIDE = 281,            /* TOKEN_DIVIDE  */
    TOKEN_MOD = 282,               /* TOKEN_MOD  */
    TOKEN_EQ = 283,                /* TOKEN_EQ  */
    TOKEN_NEQ = 284,               /* TOKEN_NEQ  */
    TOKEN_LT = 285,                /* TOKEN_LT  */
    TOKEN_GT = 286,                /* TOKEN_GT  */
    TOKEN_LEQ = 287,               /* TOKEN_LEQ  */
    TOKEN_GEQ = 288,               /* TOKEN_GEQ  */
    TOKEN_AND = 289,               /* TOKEN_AND  */
    TOKEN_OR = 290,                /* TOKEN_OR  */
    TOKEN_NOT = 291,               /* TOKEN_NOT  */
    TOKEN_ASSIGN = 292,            /* TOKEN_ASSIGN  */
    TOKEN_LPAREN = 293,            /* TOKEN_LPAREN  */
    TOKEN_RPAREN = 294,            /* TOKEN_RPAREN  */
    TOKEN_LBRACE = 295,            /* TOKEN_LBRACE  */
    TOKEN_RBRACE = 296,            /* TOKEN_RBRACE  */
    TOKEN_LBRACKET = 297,          /* TOKEN_LBRACKET  */
    TOKEN_RBRACKET = 298,          /* TOKEN_RBRACKET  */
    TOKEN_SEMICOLON = 299,         /* TOKEN_SEMICOLON  */
    TOKEN_COMMA = 300,             /* TOKEN_COMMA  */
    TOKEN_IF_WITHOUT_ELSE = 301    /* TOKEN_IF_WITHOUT_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 24 "miniJava.y"

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

#line 148 "miniJava.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_MINIJAVA_TAB_H_INCLUDED  */
