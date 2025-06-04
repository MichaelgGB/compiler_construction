#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "ast.h"
#include "tac.h"
#include "symbol_table.h" // Needed for symbol lookup to get types

// Function to start the IR generation process
void generate_ir(ProgramNode *program_root);

// Helper functions for visiting AST nodes and emitting TAC
TacOperand generate_ir_for_expression(ExpressionNode *expr_node);
void generate_ir_for_statement(StatementNode *stmt_node);
// ... and so on for other node types

#endif // IR_GENERATOR_H