#include <stdio.h>
#include <string.h>
#include "semantic_analyzer.h"
#include "ast.h"          // For node structures and enums
#include "symbol_table.h" // For symbol table operations and current_scope
#include "miniJava.tab.h" // For TOKEN_ constants

int semantic_error_count = 0;

void report_semantic_error(const char *message, int line_number)
{
    fprintf(stderr, "Semantic Error line %d: %s\n", line_number, message);
    semantic_error_count++;
}

// Main entry point
void analyze_semantics(ProgramNode *program_root)
{
    if (!program_root)
        return;
    semantic_error_count = 0;
    // init_symbol_table(); // Call this in your main driver *before* analyze_semantics
    analyze_program_node(program_root);

    if (semantic_error_count == 0)
    {
        printf("Semantic analysis successful.\n");
    }
    else
    {
        printf("Semantic analysis found %d error(s).\n", semantic_error_count);
    }
}

void analyze_program_node(ProgramNode *node)
{
    if (!node)
        return;
    analyze_class_declaration_node(node->class_decl);
}

void analyze_class_declaration_node(ClassDeclarationNode *node)
{
    if (!node)
        return;

    enter_scope();
    node->associated_scope = current_scope;

    if (!node->name || !node->name->name)
    {
        report_semantic_error("Class declaration missing name.", node->base.line_number);
    }

    VariableDeclarationListNode *vdl = node->var_decls;
    while (vdl)
    {
        analyze_variable_declaration_node(vdl->declaration);
        vdl = vdl->next;
    }

    if (node->main_method)
    {
        if (!node->main_method->name || strcmp(node->main_method->name->name, "main") != 0)
        {
            report_semantic_error("Main method must be named 'main'.", node->main_method ? node->main_method->base.line_number : node->base.line_number);
        }
        analyze_main_method_node(node->main_method);
    }
    else
    {
        report_semantic_error("No main method found in class.", node->base.line_number);
    }

    exit_scope();
}

void analyze_main_method_node(MainMethodNode *node)
{
    if (!node)
        return;

    enter_scope();
    node->associated_scope = current_scope;

    analyze_block_statement_node(node->body, TYPE_VOID); // Main is void

    exit_scope();
}

void analyze_block_statement_node(BlockStatementNode *node, TypeKind expected_return_type)
{
    if (!node)
        return;

    enter_scope();
    node->associated_scope = current_scope;

    analyze_statement_list_node(node->statements, expected_return_type);

    exit_scope();
}

void analyze_statement_list_node(StatementListNode *list, TypeKind expected_return_type)
{
    StatementListNode *current = list;
    while (current)
    {
        if (current->statement)
        {
            analyze_statement_node(current->statement, expected_return_type);
        }
        current = current->next;
    }
}

void analyze_statement_node(StatementNode *node, TypeKind expected_return_type)
{
    if (!node)
        return;

    switch (node->kind)
    {
    case STMT_KIND_VAR_DECL: // CORRECTED
        analyze_variable_declaration_node(node->data.var_decl_data); // CORRECTED
        break;
    case STMT_KIND_ASSIGNMENT: // CORRECTED
        analyze_assignment_node(node->data.assignment_data); // CORRECTED
        break;
    case STMT_KIND_IF: // CORRECTED
        analyze_if_statement_node(node->data.if_data, expected_return_type); // CORRECTED
        break;
    case STMT_KIND_WHILE: // CORRECTED
        analyze_while_statement_node(node->data.while_data, expected_return_type); // CORRECTED
        break;
    case STMT_KIND_PRINT: // CORRECTED
        analyze_print_statement_node(node->data.print_data); // CORRECTED
        break;
    case STMT_KIND_RETURN: // CORRECTED
        analyze_return_statement_node(node->data.return_data, expected_return_type); // CORRECTED
        break;
    case STMT_KIND_BLOCK: // CORRECTED
        analyze_block_statement_node(node->data.block_data, expected_return_type); // CORRECTED
        break;
    case STMT_KIND_EXPRESSION: // CORRECTED
        if (node->data.expression_data) // CORRECTED
        {
            analyze_expression_node(node->data.expression_data); // CORRECTED
        }
        break;
    default:
        report_semantic_error("Unknown statement type encountered.", node->base.line_number);
        break;
    }
}

void analyze_variable_declaration_node(VariableDeclarationNode *node)
{
    if (!node || !node->var_type || !node->identifier || !node->identifier->name) {
        report_semantic_error("Malformed variable declaration.", node ? node->base.line_number : -1);
        return;
    }

    TypeKind var_declared_type = node->var_type->kind;

    if (var_declared_type == TYPE_VOID)
    {
        report_semantic_error("Variables cannot be of type 'void'.", node->base.line_number);
    }

    if (node->initializer)
    {
        TypeKind initializer_type = analyze_expression_node(node->initializer);
        node->initializer->resolved_type = initializer_type; 
        if (initializer_type != TYPE_ERROR && var_declared_type != initializer_type)
        {
            if (!((var_declared_type == TYPE_FLOAT && initializer_type == TYPE_INT)))
            {
                char error_msg[256];
                sprintf(error_msg, "Type mismatch: cannot initialize variable '%s' of type %d with expression of type %d.",
                        node->identifier->name, var_declared_type, initializer_type);
                report_semantic_error(error_msg, node->base.line_number);
            }
        }
    }
    else if (node->is_final)
    {
        report_semantic_error("'final' variable must be initialized at declaration.", node->base.line_number);
    }

    if (!insert_symbol(node->identifier->name, var_declared_type, node->base.line_number, node->is_final, (AstNode *)node))
    {
        // Error already reported by insert_symbol
    }
    else
    {
        SymbolTableEntry *entry = lookup_symbol_in_current_scope(node->identifier->name);
        if (entry)
        {
            entry->is_initialized = (node->initializer != NULL);
        }
    }
}

void analyze_assignment_node(AssignmentNode *node)
{
    if (!node || !node->target_identifier || !node->target_identifier->name || !node->value) {
        report_semantic_error("Malformed assignment statement.", node ? node->base.line_number : -1);
        return;
    }

    SymbolTableEntry *entry = lookup_symbol(node->target_identifier->name);
    if (!entry)
    {
        char error_msg[100];
        sprintf(error_msg, "Variable '%s' not declared.", node->target_identifier->name);
        report_semantic_error(error_msg, node->base.line_number);
        node->value->resolved_type = TYPE_ERROR;
        return;
    }

    if (entry->is_final)
    {
        char error_msg[100];
        sprintf(error_msg, "Cannot assign to 'final' variable '%s'.", entry->name);
        report_semantic_error(error_msg, node->base.line_number);
    }

    TypeKind target_type;
    if (node->array_index)
    { 
        if (entry->type != TYPE_INT_ARRAY)
        { 
            char error_msg[100];
            sprintf(error_msg, "Variable '%s' is not an array.", node->target_identifier->name);
            report_semantic_error(error_msg, node->base.line_number);
            node->value->resolved_type = TYPE_ERROR;
            return;
        }
        TypeKind index_type = analyze_expression_node(node->array_index);
        node->array_index->resolved_type = index_type;
        if (index_type != TYPE_INT && index_type != TYPE_ERROR)
        {
            report_semantic_error("Array index must be an integer.", node->array_index->base.line_number);
        }
        target_type = TYPE_INT;
    }
    else
    { 
        target_type = entry->type;
    }

    TypeKind value_type = analyze_expression_node(node->value);
    node->value->resolved_type = value_type;

    if (value_type != TYPE_ERROR && target_type != value_type)
    {
        if (!((target_type == TYPE_FLOAT && value_type == TYPE_INT)))
        {
            char error_msg[256];
            sprintf(error_msg, "Type mismatch: cannot assign expression of type %d to target '%s' of type %d.",
                    value_type, node->target_identifier->name, target_type);
            report_semantic_error(error_msg, node->base.line_number);
        }
    }

    if (!node->array_index && entry)
    { 
        entry->is_initialized = 1;
    }
}

void analyze_if_statement_node(IfStatementNode *node, TypeKind expected_return_type)
{
    if (!node || !node->condition || !node->then_branch) {
         report_semantic_error("Malformed if statement.", node ? node->base.line_number : -1);
        return;
    }
    TypeKind condition_type = analyze_expression_node(node->condition);
    node->condition->resolved_type = condition_type;
    if (condition_type != TYPE_BOOLEAN && condition_type != TYPE_ERROR)
    {
        report_semantic_error("If condition must be a boolean expression.", node->condition->base.line_number);
    }
    analyze_statement_node(node->then_branch, expected_return_type);
    if (node->else_branch)
    {
        analyze_statement_node(node->else_branch, expected_return_type);
    }
}

void analyze_while_statement_node(WhileStatementNode *node, TypeKind expected_return_type)
{
    if (!node || !node->condition || !node->body) {
        report_semantic_error("Malformed while statement.", node ? node->base.line_number : -1);
        return;
    }
    TypeKind condition_type = analyze_expression_node(node->condition);
    node->condition->resolved_type = condition_type;
    if (condition_type != TYPE_BOOLEAN && condition_type != TYPE_ERROR)
    {
        report_semantic_error("While condition must be a boolean expression.", node->condition->base.line_number);
    }
    analyze_statement_node(node->body, expected_return_type);
}

void analyze_print_statement_node(PrintStatementNode *node)
{
    if (!node || !node->expression) {
        report_semantic_error("Malformed print statement.", node ? node->base.line_number : -1);
        return;
    }
    TypeKind expr_type = analyze_expression_node(node->expression);
    node->expression->resolved_type = expr_type;

    if (expr_type != TYPE_INT && expr_type != TYPE_BOOLEAN && expr_type != TYPE_CHAR &&
        expr_type != TYPE_STRING && expr_type != TYPE_FLOAT && expr_type != TYPE_ERROR)
    {
        char error_msg[100];
        sprintf(error_msg, "Expression of type %d is not directly printable.", expr_type);
        report_semantic_error(error_msg, node->expression->base.line_number);
    }
}

void analyze_return_statement_node(ReturnStatementNode *node, TypeKind expected_return_type)
{
    if (!node) return;

    if (expected_return_type == TYPE_VOID)
    {
        if (node->expression)
        {
            report_semantic_error("Cannot return a value from a void method.", node->base.line_number);
            TypeKind return_expr_type = analyze_expression_node(node->expression);
            if (node->expression) node->expression->resolved_type = return_expr_type;
        }
    }
    else
    { 
        if (!node->expression)
        {
            char error_msg[100];
            sprintf(error_msg, "Must return a value of type %d from this method.", expected_return_type);
            report_semantic_error(error_msg, node->base.line_number);
        }
        else
        {
            TypeKind return_expr_type = analyze_expression_node(node->expression);
            node->expression->resolved_type = return_expr_type;
            if (return_expr_type != TYPE_ERROR && return_expr_type != expected_return_type)
            {
                if (!((expected_return_type == TYPE_FLOAT && return_expr_type == TYPE_INT)))
                {
                    char error_msg[256];
                    sprintf(error_msg, "Type mismatch: cannot return expression of type %d from method expecting %d.",
                            return_expr_type, expected_return_type);
                    report_semantic_error(error_msg, node->expression->base.line_number);
                }
            }
        }
    }
}

TypeKind analyze_expression_node(ExpressionNode *node)
{
    if (!node)
    {
        return TYPE_ERROR;
    }

    char error_msg[256];
    TypeKind expr_type = TYPE_ERROR; 

    switch (node->kind)
    {
    case EXPR_LITERAL:
        if (!node->data.literal_expr)
        {
            report_semantic_error("Malformed literal expression.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        switch (node->data.literal_expr->kind)
        {
        case LIT_INTEGER: expr_type = TYPE_INT; break;
        case LIT_FLOAT:   expr_type = TYPE_FLOAT; break;
        case LIT_CHAR:    expr_type = TYPE_CHAR; break;
        case LIT_STRING:  expr_type = TYPE_STRING; break;
        default:
            report_semantic_error("Unknown literal kind.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        break;

    case EXPR_IDENTIFIER:
        if (!node->data.identifier_expr || !node->data.identifier_expr->name)
        {
            report_semantic_error("Malformed identifier expression.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        SymbolTableEntry *entry = lookup_symbol(node->data.identifier_expr->name);
        if (!entry)
        {
            sprintf(error_msg, "Identifier '%s' not declared.", node->data.identifier_expr->name);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        else
        {
            // if (!entry->is_initialized && entry->type != TYPE_INT_ARRAY) {} // Removed for brevity
            expr_type = entry->type;
        }
        break;

    case EXPR_ARRAY_ACCESS:
        // CORRECTED all members of array_access_expr
        if (!node->data.array_access_expr.array_name_ident || !node->data.array_access_expr.array_name_ident->name || !node->data.array_access_expr.index_expr)
        {
            report_semantic_error("Malformed array access expression.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        SymbolTableEntry *array_entry = lookup_symbol(node->data.array_access_expr.array_name_ident->name);
        if (!array_entry)
        {
            sprintf(error_msg, "Array identifier '%s' not declared.", node->data.array_access_expr.array_name_ident->name);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        else if (array_entry->type != TYPE_INT_ARRAY)
        { 
            sprintf(error_msg, "Identifier '%s' is not an array type.", array_entry->name);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        else
        {
            expr_type = TYPE_INT; 
        }

        TypeKind index_type = analyze_expression_node(node->data.array_access_expr.index_expr);
        if (node->data.array_access_expr.index_expr) node->data.array_access_expr.index_expr->resolved_type = index_type;
        if (index_type != TYPE_INT && index_type != TYPE_ERROR)
        {
            report_semantic_error("Array index must be an integer.", node->data.array_access_expr.index_expr->base.line_number);
            if (expr_type != TYPE_ERROR) expr_type = TYPE_ERROR;
        }
        break;

    case EXPR_BINARY:
    {
        if (!node->data.binary_expr.left || !node->data.binary_expr.right) {
            report_semantic_error("Malformed binary expression.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        TypeKind left_type = analyze_expression_node(node->data.binary_expr.left);
        node->data.binary_expr.left->resolved_type = left_type;
        TypeKind right_type = analyze_expression_node(node->data.binary_expr.right);
        node->data.binary_expr.right->resolved_type = right_type;

        if (left_type == TYPE_ERROR || right_type == TYPE_ERROR)
        {
            expr_type = TYPE_ERROR;
            break;
        }

        int op = node->data.binary_expr.op_token; // CORRECTED
        if (op == TOKEN_PLUS || op == TOKEN_MINUS || op == TOKEN_MULTIPLY || op == TOKEN_DIVIDE || op == TOKEN_MOD)
        { 
            if (left_type == TYPE_INT && right_type == TYPE_INT) expr_type = TYPE_INT;
            else if (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT) expr_type = TYPE_FLOAT;
            else if ((left_type == TYPE_INT && right_type == TYPE_FLOAT) || (left_type == TYPE_FLOAT && right_type == TYPE_INT)) expr_type = TYPE_FLOAT; 
            else
            {
                sprintf(error_msg, "Type mismatch for arithmetic operator %s. Operands are %d and %d.", get_token_name_from_int(op), left_type, right_type); // CORRECTED function call
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else if (op == TOKEN_LT || op == TOKEN_LEQ || op == TOKEN_GT || op == TOKEN_GEQ)
        { 
            if ((left_type == TYPE_INT && right_type == TYPE_INT) ||
                (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT) ||
                (left_type == TYPE_CHAR && right_type == TYPE_CHAR) ||
                ((left_type == TYPE_INT || left_type == TYPE_FLOAT) && (right_type == TYPE_INT || right_type == TYPE_FLOAT)) 
            ) { expr_type = TYPE_BOOLEAN; }
            else
            {
                sprintf(error_msg, "Type mismatch for relational operator %s. Operands are %d and %d.", get_token_name_from_int(op), left_type, right_type); // CORRECTED function call
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else if (op == TOKEN_EQ || op == TOKEN_NEQ)
        { 
            if (left_type == right_type && (left_type == TYPE_INT || left_type == TYPE_BOOLEAN || left_type == TYPE_CHAR || left_type == TYPE_FLOAT || left_type == TYPE_STRING))
            { expr_type = TYPE_BOOLEAN; }
            else if (((left_type == TYPE_INT && right_type == TYPE_FLOAT) || (left_type == TYPE_FLOAT && right_type == TYPE_INT)))
            { expr_type = TYPE_BOOLEAN; }
            else
            {
                sprintf(error_msg, "Type mismatch for equality operator %s. Operands are %d and %d.", get_token_name_from_int(op), left_type, right_type); // CORRECTED function call
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else if (op == TOKEN_AND || op == TOKEN_OR)
        { 
            if (left_type == TYPE_BOOLEAN && right_type == TYPE_BOOLEAN) expr_type = TYPE_BOOLEAN;
            else
            {
                sprintf(error_msg, "Logical operator %s requires boolean operands. Got %d and %d.", get_token_name_from_int(op), left_type, right_type); // CORRECTED function call
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else
        {
            sprintf(error_msg, "Unsupported binary operator %s.", get_token_name_from_int(op)); // CORRECTED function call
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        break;
    }

    case EXPR_UNARY:
    {
        if(!node->data.unary_expr.operand) {
            report_semantic_error("Malformed unary expression.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        TypeKind operand_type = analyze_expression_node(node->data.unary_expr.operand);
        node->data.unary_expr.operand->resolved_type = operand_type;
        if (operand_type == TYPE_ERROR)
        {
            expr_type = TYPE_ERROR;
            break;
        }
        int op = node->data.unary_expr.op_token; // CORRECTED
        if (op == TOKEN_MINUS)
        { 
            if (operand_type == TYPE_INT) expr_type = TYPE_INT;
            else if (operand_type == TYPE_FLOAT) expr_type = TYPE_FLOAT;
            else
            {
                report_semantic_error("Operand for unary minus must be numeric (int or float).", node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else if (op == TOKEN_NOT)
        { 
            if (operand_type == TYPE_BOOLEAN) expr_type = TYPE_BOOLEAN;
            else
            {
                report_semantic_error("Operand for logical NOT '!' must be boolean.", node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else
        {
            sprintf(error_msg, "Unsupported unary operator %s.", get_token_name_from_int(op)); // CORRECTED function call
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        break;
    }

    case EXPR_PARENTHESIZED:
        if (!node->data.parenthesized_expr_val) // CORRECTED
        {
            report_semantic_error("Malformed parenthesized expression.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        expr_type = analyze_expression_node(node->data.parenthesized_expr_val); // CORRECTED
        break;

    case EXPR_BOOLEAN_TRUE: // CORRECTED
    case EXPR_BOOLEAN_FALSE: // CORRECTED
        expr_type = TYPE_BOOLEAN;
        break;

    default:
        report_semantic_error("Unknown expression kind encountered in analysis.", node->base.line_number);
        expr_type = TYPE_ERROR;
        break;
    }

    node->resolved_type = expr_type;
    return expr_type;
}