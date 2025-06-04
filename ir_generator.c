#include "ir_generator.h"
#include "tac.h"
#include "symbol_table.h"
#include "miniJava.tab.h" // Assuming this contains your TOKEN_ definitions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for internal helper functions
// Removed 'static' as they are likely declared globally in ir_generator.h
TacOperand generate_ir_for_expression(ExpressionNode *expr_node);
void generate_ir_for_statement(StatementNode *stmt_node);
void generate_ir_for_statement_list(StatementListNode *stmt_list_node);
void generate_ir_for_variable_declaration(VariableDeclarationNode *var_decl_node);
void generate_ir_for_assignment(AssignmentNode *assign_node);
void generate_ir_for_if_statement(IfStatementNode *if_stmt_node);
void generate_ir_for_while_statement(WhileStatementNode *while_stmt_node);
void generate_ir_for_print_statement(PrintStatementNode *print_stmt_node);
void generate_ir_for_return_statement(ReturnStatementNode *return_stmt_node);
void generate_ir_for_block_statement(BlockStatementNode *block_stmt_node);

void generate_ir(ProgramNode *program_root)
{
    if (!program_root)
    {
        fprintf(stderr, "IR Gen Error: Program root is NULL.\n");
        return;
    }

    // Access `class_decl` directly from `program_root`
    if (program_root->class_decl && program_root->class_decl->main_method)
    {
        MainMethodNode *main_method = program_root->class_decl->main_method;

        // *** CRITICAL ADDITION: Enter the main method's scope ***
        // This ensures that 'current_scope' is set to the correct scope
        // where 'num' (and other local variables) are declared.
        enter_scope(); // Call to symbol_table.c

        emit_tac(TAC_LABEL, create_tac_label_operand("main"), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);
        if (main_method->body)
        {
            generate_ir_for_block_statement(main_method->body);
        }

        // *** CRITICAL ADDITION: Exit the main method's scope ***
        // This restores 'current_scope' to its parent (the global scope).
        exit_scope(); // Call to symbol_table.c
    }
    else
    {
        fprintf(stderr, "IR Gen Error: No main method found.\n");
    }
}

// --- Expression Generation ---
TacOperand generate_ir_for_expression(ExpressionNode *expr_node)
{
    TacOperand result_op = create_tac_none_operand();

    if (!expr_node)
    {
        fprintf(stderr, "IR Gen Error: NULL expression node.\n");
        return result_op;
    }

    switch (expr_node->base.type)
    {
    case NODE_EXPRESSION_LITERAL:
    {
        LiteralNode *lit = expr_node->data.literal_expr;
        if (!lit)
        {
            fprintf(stderr, "IR Gen Error: NULL literal in expression at line %d.\n", expr_node->base.line_number);
            return result_op;
        }

        switch (lit->kind)
        {
        case LIT_INTEGER:
            result_op = create_tac_int_literal_operand(lit->data.int_val);
            break;
        case LIT_FLOAT:
            result_op = create_tac_float_literal_operand(lit->data.float_val);
            break;
        case LIT_CHAR:
            result_op = create_tac_char_literal_operand(lit->data.char_val);
            break;
        case LIT_STRING:
            result_op = create_tac_string_literal_operand(lit->data.string_val);
            break;
        default:
            fprintf(stderr, "IR Gen Error: Unknown literal type %d at line %d.\n", lit->kind, lit->base.line_number);
            break;
        }
        break;
    }
    case NODE_EXPRESSION_BOOLEAN:
    {
        result_op = create_tac_bool_literal_operand(expr_node->kind == EXPR_TRUE ? 1 : 0);
        break;
    }
    case NODE_EXPRESSION_IDENTIFIER:
    {
        IdentifierNode *id_node = expr_node->data.identifier_expr;
        if (!id_node)
        {
            fprintf(stderr, "IR Gen Error: NULL identifier in expression at line %d.\n", expr_node->base.line_number);
            return result_op;
        }

        SymbolTableEntry *entry = lookup_symbol(id_node->name);
        if (!entry)
        {
            fprintf(stderr, "IR Gen Error: Identifier '%s' not found at line %d.\n", id_node->name, expr_node->base.line_number);
            exit(EXIT_FAILURE);
        }
        result_op = create_tac_var_operand(entry->name, entry->type);
        break;
    }
    case NODE_EXPRESSION_BINARY:
    {
        TacOperand left_op = generate_ir_for_expression(expr_node->data.binary_expr.left);
        TacOperand right_op = generate_ir_for_expression(expr_node->data.binary_expr.right);
        TacOperand temp_res = create_tac_temp_operand(expr_node->resolved_type);

        switch (expr_node->data.binary_expr.op)
        {
        case TOKEN_PLUS:
            emit_tac(TAC_ADD, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_MINUS:
            emit_tac(TAC_SUB, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_MULTIPLY:
            emit_tac(TAC_MUL, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_DIVIDE:
            emit_tac(TAC_DIV, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_MOD:
            emit_tac(TAC_MOD, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_LT:
            emit_tac(TAC_LT, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_GT:
            emit_tac(TAC_GT, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_LEQ:
            emit_tac(TAC_LEQ, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_GEQ:
            emit_tac(TAC_GEQ, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_EQ:
            emit_tac(TAC_EQ, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_NEQ:
            emit_tac(TAC_NEQ, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_AND:
            emit_tac(TAC_AND, temp_res, left_op, right_op, temp_res.type);
            break;
        case TOKEN_OR:
            emit_tac(TAC_OR, temp_res, left_op, right_op, temp_res.type);
            break;
        default:
            fprintf(stderr, "IR Gen Error: Unknown binary operator %d at line %d.\n", expr_node->data.binary_expr.op, expr_node->base.line_number);
            break;
        }
        result_op = temp_res;
        break;
    }
    case NODE_EXPRESSION_UNARY:
    {
        TacOperand operand_op = generate_ir_for_expression(expr_node->data.unary_expr.operand);
        TacOperand temp_res = create_tac_temp_operand(expr_node->resolved_type);

        switch (expr_node->data.unary_expr.op)
        {
        case TOKEN_MINUS:
            emit_tac(TAC_UMINUS, temp_res, operand_op, create_tac_none_operand(), temp_res.type);
            break;
        case TOKEN_NOT:
            emit_tac(TAC_NOT, temp_res, operand_op, create_tac_none_operand(), temp_res.type);
            break;
        default:
            fprintf(stderr, "IR Gen Error: Unknown unary operator %d at line %d.\n", expr_node->data.unary_expr.op, expr_node->base.line_number);
            break;
        }
        result_op = temp_res;
        break;
    }
    case NODE_EXPRESSION_PARENTHESIZED:
    {
        result_op = generate_ir_for_expression(expr_node->data.parenthesized_expr);
        break;
    }
    case NODE_EXPRESSION_ARRAY_ACCESS:
    {
        IdentifierNode *array_id_node = expr_node->data.array_access_expr.array_base;
        ExpressionNode *index_expr = expr_node->data.array_access_expr.index;

        if (!array_id_node || !index_expr)
        {
            fprintf(stderr, "IR Gen Error: Malformed array access at line %d.\n", expr_node->base.line_number);
            exit(EXIT_FAILURE);
        }

        TacOperand array_op = create_tac_var_operand(array_id_node->name, TYPE_INT_ARRAY); // Array always TYPE_INT_ARRAY in MiniJava
        TacOperand index_op = generate_ir_for_expression(index_expr);

        result_op = create_tac_temp_operand(expr_node->resolved_type);
        emit_tac(TAC_ARRAY_LOAD, result_op, array_op, index_op, result_op.type);
        break;
    }
    default:
        fprintf(stderr, "IR Gen Error: Unhandled expression type %d at line %d.\n", expr_node->base.type, expr_node->base.line_number);
        exit(EXIT_FAILURE);
    }
    return result_op;
}

// --- Statement Generation ---
void generate_ir_for_statement(StatementNode *stmt_node)
{
    if (!stmt_node)
    {
        fprintf(stderr, "IR Gen Error: NULL statement node.\n");
        return;
    }

    switch (stmt_node->base.type)
    {
    case NODE_STATEMENT_VAR_DECL:
        generate_ir_for_variable_declaration(stmt_node->data.var_decl_stmt);
        break;
    case NODE_STATEMENT_ASSIGNMENT:
        generate_ir_for_assignment(stmt_node->data.assignment_stmt);
        break;
    case NODE_STATEMENT_IF:
        generate_ir_for_if_statement(stmt_node->data.if_stmt);
        break;
    case NODE_STATEMENT_WHILE:
        generate_ir_for_while_statement(stmt_node->data.while_stmt);
        break;
    case NODE_STATEMENT_PRINT:
        generate_ir_for_print_statement(stmt_node->data.print_stmt);
        break;
    case NODE_STATEMENT_RETURN:
        generate_ir_for_return_statement(stmt_node->data.return_stmt);
        break;
    case NODE_STATEMENT_BLOCK:
        generate_ir_for_block_statement(stmt_node->data.block_stmt);
        break;
    default:
        fprintf(stderr, "IR Gen Error: Unhandled statement type %d at line %d.\n", stmt_node->base.type, stmt_node->base.line_number);
        exit(EXIT_FAILURE);
    }
}

void generate_ir_for_statement_list(StatementListNode *stmt_list_node)
{
    StatementListNode *current = stmt_list_node;
    while (current)
    {
        if (current->statement)
        {
            generate_ir_for_statement(current->statement);
        }
        current = current->next;
    }
}

// --- Variable Declaration Generation ---
void generate_ir_for_variable_declaration(VariableDeclarationNode *var_decl_node)
{
    if (!var_decl_node)
        return;

    TacOperand var_op = create_tac_var_operand(var_decl_node->identifier->name, var_decl_node->var_type->kind);

    if (var_decl_node->initializer)
    {
        TacOperand initializer_op = generate_ir_for_expression(var_decl_node->initializer);
        emit_tac(TAC_ASSIGN, var_op, initializer_op, create_tac_none_operand(), initializer_op.type);
    }
}

// --- Assignment Generation ---
void generate_ir_for_assignment(AssignmentNode *assign_node)
{
    if (!assign_node)
        return;

    TacOperand value_op = generate_ir_for_expression(assign_node->value);
    TacOperand target_op;

    if (assign_node->array_index)
    { // Assignment to an array element (e.g., arr[i] = val)
        IdentifierNode *array_id_node = assign_node->target_identifier;
        ExpressionNode *index_expr = assign_node->array_index;

        if (!array_id_node || !index_expr)
        {
            fprintf(stderr, "IR Gen Error: Malformed array assignment at line %d.\n", assign_node->base.line_number);
            exit(EXIT_FAILURE);
        }

        TacOperand array_op = create_tac_var_operand(array_id_node->name, TYPE_INT_ARRAY);
        TacOperand index_op = generate_ir_for_expression(index_expr);

        emit_tac(TAC_ARRAY_STORE, value_op, array_op, index_op, value_op.type);
    }
    else
    { // Simple variable assignment (e.g., x = 5)
        SymbolTableEntry *entry = lookup_symbol(assign_node->target_identifier->name);
        if (!entry)
        {
            fprintf(stderr, "IR Gen Error: Target identifier '%s' not found for assignment at line %d.\n", assign_node->target_identifier->name, assign_node->base.line_number);
            exit(EXIT_FAILURE);
        }
        target_op = create_tac_var_operand(entry->name, entry->type);
        emit_tac(TAC_ASSIGN, target_op, value_op, create_tac_none_operand(), value_op.type);
    }
}

// --- If Statement Generation ---
void generate_ir_for_if_statement(IfStatementNode *if_stmt_node)
{
    if (!if_stmt_node)
        return;

    TacOperand cond_op = generate_ir_for_expression(if_stmt_node->condition);

    char *else_label = generate_new_label_name();
    char *end_if_label = generate_new_label_name();

    emit_tac(TAC_IF_NOT_GOTO, cond_op, create_tac_label_operand(else_label), create_tac_none_operand(), cond_op.type);

    generate_ir_for_statement(if_stmt_node->then_branch);
    emit_tac(TAC_GOTO, create_tac_label_operand(end_if_label), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);

    emit_tac(TAC_LABEL, create_tac_label_operand(else_label), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);
    if (if_stmt_node->else_branch)
    {
        generate_ir_for_statement(if_stmt_node->else_branch);
    }
    emit_tac(TAC_LABEL, create_tac_label_operand(end_if_label), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);
}

// --- While Statement Generation ---
void generate_ir_for_while_statement(WhileStatementNode *while_stmt_node)
{
    if (!while_stmt_node)
        return;

    char *loop_start_label = generate_new_label_name();
    char *loop_end_label = generate_new_label_name();

    emit_tac(TAC_LABEL, create_tac_label_operand(loop_start_label), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);

    TacOperand cond_op = generate_ir_for_expression(while_stmt_node->condition);
    emit_tac(TAC_IF_NOT_GOTO, cond_op, create_tac_label_operand(loop_end_label), create_tac_none_operand(), cond_op.type);

    generate_ir_for_statement(while_stmt_node->body);

    emit_tac(TAC_GOTO, create_tac_label_operand(loop_start_label), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);

    emit_tac(TAC_LABEL, create_tac_label_operand(loop_end_label), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);
}

// --- Print Statement Generation ---
void generate_ir_for_print_statement(PrintStatementNode *print_stmt_node)
{
    if (!print_stmt_node || !print_stmt_node->expression)
        return;

    TacOperand expr_op = generate_ir_for_expression(print_stmt_node->expression);
    emit_tac(TAC_PRINT, expr_op, create_tac_none_operand(), create_tac_none_operand(), expr_op.type);
}

// --- Return Statement Generation ---
void generate_ir_for_return_statement(ReturnStatementNode *return_stmt_node)
{
    if (!return_stmt_node)
        return;

    if (return_stmt_node->expression)
    {
        TacOperand expr_op = generate_ir_for_expression(return_stmt_node->expression);
        emit_tac(TAC_RETURN, expr_op, create_tac_none_operand(), create_tac_none_operand(), expr_op.type);
    }
    else
    {
        emit_tac(TAC_RETURN, create_tac_none_operand(), create_tac_none_operand(), create_tac_none_operand(), TYPE_VOID);
    }
}

// --- Block Statement Generation ---
void generate_ir_for_block_statement(BlockStatementNode *block_stmt_node)
{
    if (!block_stmt_node)
        return;

    // *** CRITICAL ADDITION: Enter the new block's scope ***
    // This creates a new, nested scope for statements within this block.
    enter_scope(); // Call to symbol_table.c

    generate_ir_for_statement_list(block_stmt_node->statements);

    // *** CRITICAL ADDITION: Exit the current block's scope ***
    // This returns 'current_scope' to the parent scope where this block is defined.
    exit_scope(); // Call to symbol_table.c
}