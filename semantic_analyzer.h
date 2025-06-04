#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "ast.h"
#include "symbol_table.h" // We'll be using symbol table operations

// Main entry point for semantic analysis
void analyze_semantics(ProgramNode *program_root);

// Analysis functions for different AST node types
void analyze_program_node(ProgramNode *node);
void analyze_class_declaration_node(ClassDeclarationNode *node);
void analyze_main_method_node(MainMethodNode *node);
void analyze_block_statement_node(BlockStatementNode *node, TypeKind expected_return_type); // Added expected_return_type
void analyze_statement_list_node(StatementListNode *list, TypeKind expected_return_type);
void analyze_statement_node(StatementNode *node, TypeKind expected_return_type);

void analyze_variable_declaration_node(VariableDeclarationNode *node);
void analyze_assignment_node(AssignmentNode *node);
void analyze_if_statement_node(IfStatementNode *node, TypeKind expected_return_type);
void analyze_while_statement_node(WhileStatementNode *node, TypeKind expected_return_type);
void analyze_print_statement_node(PrintStatementNode *node);
void analyze_return_statement_node(ReturnStatementNode *node, TypeKind expected_return_type);
int is_lhs_of_assignment(ExpressionNode *node);

// Expression analysis functions return the type of the expression
TypeKind analyze_expression_node(ExpressionNode *node);

// Helper for error reporting
extern int semantic_error_count; // Keep track of errors
void report_semantic_error(const char *message, int line_number);

#endif // SEMANTIC_ANALYZER_H