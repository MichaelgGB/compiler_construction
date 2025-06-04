%{
#include "ast.h"
#include "miniJava.tab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For strdup

// Declare yylval to be of type YYSTYPE, as defined in miniJava.tab.h
extern YYSTYPE yylval;

// Declare yyin (input file pointer for Flex)
// extern FILE *yyin; // Flex typically declares this itself unless you have specific needs

// Declare yylineno (line number counter from Flex)
// extern int yylineno; // Flex declares this if %option yylineno is used

// Declare yyerror function, which will be defined in miniJava.tab.c (from Bison)
// extern void yyerror(const char *s); // Usually called from parser, not directly by lexer logic here
%}

/* Flex Options */
%option noyywrap    
%option yylineno    
%option nounput     
%option noinput     


%%


[ \t\r]+                { /* Ignore spaces, tabs, carriage returns */ }
\n                      { /* yylineno is incremented automatically by %option yylineno */ }
"//".* { /* Single-line comments */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /* Multi-line comments, non-greedy */ }



"class"                 { return TOKEN_CLASS; }
"public"                { return TOKEN_PUBLIC; }
"static"                { return TOKEN_STATIC; }
"void"                  { return TOKEN_VOID; }
"int"                   { return TOKEN_INT; }
"char"                  { return TOKEN_CHAR; }
"boolean"               { return TOKEN_BOOLEAN; }
"if"                    { return TOKEN_IF; }
"else"                  { return TOKEN_ELSE; }
"while"                 { return TOKEN_WHILE; }
"return"                { return TOKEN_RETURN; }
"true"                  { return TOKEN_TRUE; }
"false"                 { return TOKEN_FALSE; }
"final"                 { return TOKEN_FINAL; }
"print"                 { return TOKEN_PRINT; }


[a-zA-Z_][a-zA-Z0-9_]* { yylval.string_val = strdup(yytext); return TOKEN_ID; }


[0-9]+                  { yylval.int_val = atoi(yytext); return TOKEN_INT_LIT; }
[0-9]+\.[0-9]+          { yylval.float_val = atof(yytext); return TOKEN_FLOAT_LIT; }


\'(\\([ntrfbva\\\'])|[^\'\\])\' {
                            if (yytext[1] == '\\') { // Check for escape sequence
                                switch (yytext[2]) {
                                    case 'n': yylval.char_val = '\n'; break;
                                    case 't': yylval.char_val = '\t'; break;
                                    case 'r': yylval.char_val = '\r'; break;
                                    case 'f': yylval.char_val = '\f'; break;
                                    case 'b': yylval.char_val = '\b'; break;
                                    case 'v': yylval.char_val = '\v'; break;
                                    case 'a': yylval.char_val = '\a'; break;
                                    case '\\': yylval.char_val = '\\'; break;
                                    case '\'': yylval.char_val = '\''; break;
                                    default: yylval.char_val = yytext[2]; /* Should not happen with this regex but as fallback */ break;
                                }
                            } else {
                                yylval.char_val = yytext[1]; // Not an escape, just the character
                            }
                            return TOKEN_CHAR_LIT;
                        }


\"                             {
                                    char buffer[1024]; // Assuming max string length
                                    int i = 0, j = 0;
                                    for (i = 1; yytext[i] != '\"' && yytext[i] != '\0' && j < sizeof(buffer) -1; i++) {
                                        if (yytext[i] == '\\') {
                                            i++; // Skip backslash, process next char
                                            switch (yytext[i]) {
                                                case 'n': buffer[j++] = '\n'; break;
                                                case 't': buffer[j++] = '\t'; break;
                                                case 'r': buffer[j++] = '\r'; break;
                                                case '"': buffer[j++] = '"'; break;
                                                case '\\': buffer[j++] = '\\'; break;
                                                default: buffer[j++] = yytext[i]; // Store escaped char as is
                                            }
                                        } else {
                                            buffer[j++] = yytext[i];
                                        }
                                    }
                                    buffer[j] = '\0';
                                    yylval.string_val = strdup(buffer);
                                    return TOKEN_STRING_LIT;
                                }


"+"                     { return TOKEN_PLUS; }
"-"                     { return TOKEN_MINUS; }
"*"                     { return TOKEN_MULTIPLY; }
"/"                     { return TOKEN_DIVIDE; }
"%"                     { return TOKEN_MOD; }
"=="                    { return TOKEN_EQ; }
"!="                    { return TOKEN_NEQ; }
"<"                     { return TOKEN_LT; }
">"                     { return TOKEN_GT; }
"<="                    { return TOKEN_LEQ; }
">="                    { return TOKEN_GEQ; }
"&&"                    { return TOKEN_AND; }
"||"                    { return TOKEN_OR; }
"!"                     { return TOKEN_NOT; }
"="                     { return TOKEN_ASSIGN; }


"("                     { return TOKEN_LPAREN; }
")"                     { return TOKEN_RPAREN; }
"{"                     { return TOKEN_LBRACE; }
"}"                     { return TOKEN_RBRACE; }
"["                     { return TOKEN_LBRACKET; }
"]"                     { return TOKEN_RBRACKET; }
";"                     { return TOKEN_SEMICOLON; }
","                     { return TOKEN_COMMA; }

.                       {
                            fprintf(stderr, "Lexical error at line %d: Unexpected character '%s'\n", yylineno, yytext);
                            // Depending on desired behavior, you might not want to exit immediately
                            // For a compiler, you might want to report multiple errors
                            // For now, we'll keep the exit:
                            exit(1); 
                        }

%%

// If you use %option noyywrap, you don't need to define yywrap().
// The main function is typically in the .y file or a separate C file.
// int main(int argc, char **argv) {
//     if (argc > 1) yyin = fopen(argv[1], "r");
//     else yyin = stdin;
//     yylex();
//     return 0;
// }