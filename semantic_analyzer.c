#include <stdio.h>
#include <string.h>
#include "semantic_analyzer.h"
#include "ast.h"          // For node structures and enums
#include "symbol_table.h" // For symbol table operations
#include "miniJava.tab.h"

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
    current_scope = NULL; // Initialize current_scope for the whole analysis pass
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

    enter_scope(); // Class scope

    // Check class name
    if (!node->name || !node->name->name)
    {
        report_semantic_error("Class declaration missing name.", node->base.line_number);
    }

    // Process variable declarations (fields) in the class
    VariableDeclarationListNode *vdl = node->var_decls;
    while (vdl)
    {
        analyze_variable_declaration_node(vdl->declaration);
        vdl = vdl->next;
    }

    if (node->main_method)
    {
        if (strcmp(node->main_method->name->name, "main") != 0)
        {
            report_semantic_error("Main method must be named 'main'.", node->main_method->base.line_number);
        }
        analyze_main_method_node(node->main_method);
    }
    else
    {
        report_semantic_error("No main method found.", node->base.line_number);
    }

    exit_scope(); // Exit class scope
}

void analyze_main_method_node(MainMethodNode *node)
{
    if (!node)
        return;

    enter_scope(); // Scope for main method (parameters and local variables)

    // Analyze the body of the main method.
    // The main method in Java is void.
    analyze_block_statement_node(node->body, TYPE_VOID);

    exit_scope(); // Exit main method scope
}

void analyze_block_statement_node(BlockStatementNode *node, TypeKind expected_return_type)
{
    if (!node)
        return;

    // Each block can have its own scope for local variables
    enter_scope();
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
    case STMT_VAR_DECL:
        analyze_variable_declaration_node(node->data.var_decl_stmt);
        break;
    case STMT_ASSIGNMENT:
        analyze_assignment_node(node->data.assignment_stmt);
        break;
    case STMT_IF:
        analyze_if_statement_node(node->data.if_stmt, expected_return_type);
        break;
    case STMT_WHILE:
        analyze_while_statement_node(node->data.while_stmt, expected_return_type);
        break;
    case STMT_PRINT:
        analyze_print_statement_node(node->data.print_stmt);
        break;
    case STMT_RETURN:
        analyze_return_statement_node(node->data.return_stmt, expected_return_type);
        break;
    case STMT_BLOCK:
        analyze_block_statement_node(node->data.block_stmt, expected_return_type);
        break;
    case STMT_EXPRESSION:
        if (node->data.expression_stmt)
        {
            analyze_expression_node(node->data.expression_stmt);
        }
        break;
    default:
        report_semantic_error("Unknown statement type encountered.", node->base.line_number);
        break;
    }
}

void analyze_variable_declaration_node(VariableDeclarationNode *node)
{
    if (!node)
        return;

    TypeKind var_declared_type = node->var_type->kind;

    if (var_declared_type == TYPE_VOID)
    {
        report_semantic_error("Variables cannot be of type 'void'.", node->base.line_number);
    }

    // Check for initializer
    if (node->initializer)
    {
        TypeKind initializer_type = analyze_expression_node(node->initializer);
        if (initializer_type != TYPE_ERROR && var_declared_type != initializer_type)
        {
            char error_msg[256];
            sprintf(error_msg, "Type mismatch: cannot initialize variable '%s' of type %d with expression of type %d.",
                    node->identifier->name, var_declared_type, initializer_type);
            report_semantic_error(error_msg, node->base.line_number);
        }
    }
    else if (node->is_final)
    {
        report_semantic_error("'final' variable must be initialized at declaration.", node->base.line_number);
    }

    // Insert into symbol table
    if (insert_symbol(node->identifier->name, var_declared_type, node->base.line_number, node->is_final, (AstNode *)node))
    {
        // Mark initialization status
        SymbolTableEntry *entry = lookup_symbol_in_current_scope(node->identifier->name);
        if (entry)
        {
            entry->is_initialized = (node->initializer != NULL) ? 1 : 0;
        }
    }
}

void analyze_assignment_node(AssignmentNode *node)
{
    if (!node)
        return;

    SymbolTableEntry *entry = lookup_symbol(node->target_identifier->name);
    if (!entry)
    {
        char error_msg[100];
        sprintf(error_msg, "Variable '%s' not declared.", node->target_identifier->name);
        report_semantic_error(error_msg, node->base.line_number);
        return;
    }

    if (entry->is_final)
    {
        char error_msg[100];
        sprintf(error_msg, "Cannot assign to 'final' variable '%s'.", node->target_identifier->name);
        report_semantic_error(error_msg, node->base.line_number);
    }

    TypeKind target_type;
    if (node->array_index)
    { // Array assignment: foo[idx] = val
        if (entry->type != TYPE_INT_ARRAY)
        {
            char error_msg[100];
            sprintf(error_msg, "Variable '%s' is not an array.", node->target_identifier->name);
            report_semantic_error(error_msg, node->base.line_number);
            return;
        }
        TypeKind index_type = analyze_expression_node(node->array_index);
        if (index_type != TYPE_INT && index_type != TYPE_ERROR)
        {
            report_semantic_error("Array index must be an integer.", node->base.line_number);
        }
        target_type = TYPE_INT; // Elements of int[] are int
    }
    else
    { // Simple variable assignment: foo = val
        target_type = entry->type;
    }

    TypeKind value_type = analyze_expression_node(node->value);

    if (value_type != TYPE_ERROR && target_type != value_type)
    {
        char error_msg[256];
        sprintf(error_msg, "Type mismatch: cannot assign expression of type %d to target of type %d for '%s'.",
                value_type, target_type, node->target_identifier->name);
        report_semantic_error(error_msg, node->base.line_number);
    }

    // Mark variable as initialized after successful assignment
    if (!node->array_index && entry)
    {
        entry->is_initialized = 1;
    }
}

void analyze_if_statement_node(IfStatementNode *node, TypeKind expected_return_type)
{
    if (!node)
        return;
    TypeKind condition_type = analyze_expression_node(node->condition);
    if (condition_type != TYPE_BOOLEAN && condition_type != TYPE_ERROR)
    {
        report_semantic_error("If condition must be a boolean expression.", node->base.line_number);
    }
    analyze_statement_node(node->then_branch, expected_return_type);
    if (node->else_branch)
    {
        analyze_statement_node(node->else_branch, expected_return_type);
    }
}

void analyze_while_statement_node(WhileStatementNode *node, TypeKind expected_return_type)
{
    if (!node)
        return;
    TypeKind condition_type = analyze_expression_node(node->condition);
    if (condition_type != TYPE_BOOLEAN && condition_type != TYPE_ERROR)
    {
        report_semantic_error("While condition must be a boolean expression.", node->base.line_number);
    }
    analyze_statement_node(node->body, expected_return_type);
}

void analyze_print_statement_node(PrintStatementNode *node)
{
    if (!node)
        return;
    TypeKind expr_type = analyze_expression_node(node->expression);
    // Check if expr_type is printable
    if (expr_type != TYPE_INT && expr_type != TYPE_BOOLEAN && expr_type != TYPE_CHAR &&
        expr_type != TYPE_STRING && expr_type != TYPE_FLOAT && expr_type != TYPE_ERROR)
    {
        char error_msg[100];
        sprintf(error_msg, "Expression of type %d is not printable.", expr_type);
        report_semantic_error(error_msg, node->base.line_number);
    }
}

void analyze_return_statement_node(ReturnStatementNode *node, TypeKind expected_return_type)
{
    if (!node)
        return;

    if (expected_return_type == TYPE_VOID)
    {
        if (node->expression)
        {
            report_semantic_error("Cannot return a value from a void method.", node->base.line_number);
            analyze_expression_node(node->expression); // Analyze for other errors
        }
    }
    else
    { // Non-void method
        if (!node->expression)
        {
            char error_msg[100];
            sprintf(error_msg, "Must return a value of type %d from this method.", expected_return_type);
            report_semantic_error(error_msg, node->base.line_number);
        }
        else
        {
            TypeKind return_expr_type = analyze_expression_node(node->expression);
            if (return_expr_type != TYPE_ERROR && return_expr_type != expected_return_type)
            {
                char error_msg[256];
                sprintf(error_msg, "Type mismatch: cannot return expression of type %d from method expecting %d.",
                        return_expr_type, expected_return_type);
                report_semantic_error(error_msg, node->base.line_number);
            }
        }
    }
}

TypeKind analyze_expression_node(ExpressionNode *node)
{
    if (!node)
    {
        return TYPE_ERROR; // Or handle as a fatal error
    }

    char error_msg[256];             // For sprintf messages
    TypeKind expr_type = TYPE_ERROR; // Initialize with error type

    switch (node->kind)
    {
    case EXPR_LITERAL:
        if (!node->data.literal_expr)
        {
            // If the literal expression data is missing, it's an internal error
            expr_type = TYPE_ERROR;
            break; // Exit switch and set resolved_type below
        }
        switch (node->data.literal_expr->kind)
        {
        case LIT_INTEGER:
            expr_type = TYPE_INT;
            break;
        case LIT_FLOAT:
            expr_type = TYPE_FLOAT;
            break;
        case LIT_CHAR:
            expr_type = TYPE_CHAR;
            break;
        case LIT_STRING:
            expr_type = TYPE_STRING;
            break;
        default:
            report_semantic_error("Unknown literal kind.", node->base.line_number);
            expr_type = TYPE_ERROR;
            break;
        }
        break; // Exit EXPR_LITERAL case

    case EXPR_IDENTIFIER:
    {
        if (!node->data.identifier_expr || !node->data.identifier_expr->name)
        {
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
            // Check if variable is initialized before use (good semantic check!)
            if (!entry->is_initialized)
            {
                sprintf(error_msg, "Variable '%s' might not have been initialized.", entry->name);
                report_semantic_error(error_msg, node->base.line_number);
                // This is a warning, not necessarily a type error that stops IR generation,
                // so we still return its declared type.
            }
            expr_type = entry->type; // Get type from symbol table
        }
        break;
    }

    case EXPR_ARRAY_ACCESS:
    {
        if (!node->data.array_access_expr.array_base || !node->data.array_access_expr.array_base->name)
        {
            expr_type = TYPE_ERROR;
            break;
        }

        SymbolTableEntry *array_entry = lookup_symbol(node->data.array_access_expr.array_base->name);
        if (!array_entry)
        {
            sprintf(error_msg, "Array identifier '%s' not declared.", node->data.array_access_expr.array_base->name);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        else if (array_entry->type != TYPE_INT_ARRAY) // Assuming only int arrays
        {
            sprintf(error_msg, "Identifier '%s' is not an array.", array_entry->name);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        else
        {
            // Check if array is initialized
            if (!array_entry->is_initialized)
            {
                sprintf(error_msg, "Array '%s' might not have been initialized.", array_entry->name);
                report_semantic_error(error_msg, node->base.line_number);
            }
            expr_type = TYPE_INT; // Element type of int array is INT
        }

        // Analyze the index expression's type
        TypeKind index_type = analyze_expression_node(node->data.array_access_expr.index);
        if (index_type != TYPE_INT && index_type != TYPE_ERROR)
        {
            report_semantic_error("Array index must be an integer.", node->base.line_number);
            // If index is wrong type, the overall expression is still an error.
            // If expr_type was already TYPE_ERROR, keep it.
            if (expr_type != TYPE_ERROR)
            {
                expr_type = TYPE_ERROR;
            }
        }
        // If index_type is TYPE_ERROR, it will propagate up.
        break;
    }

    case EXPR_BINARY:
    {
        TypeKind left_type = analyze_expression_node(node->data.binary_expr.left);
        TypeKind right_type = analyze_expression_node(node->data.binary_expr.right);

        if (left_type == TYPE_ERROR || right_type == TYPE_ERROR)
        {
            expr_type = TYPE_ERROR; // Propagate error
            break;
        }

        int op = node->data.binary_expr.op;

        // Arithmetic operations: +, -, *, /, %
        if (op == TOKEN_PLUS || op == TOKEN_MINUS || op == TOKEN_MULTIPLY || op == TOKEN_DIVIDE || op == TOKEN_MOD)
        {
            if (left_type == TYPE_INT && right_type == TYPE_INT)
            {
                expr_type = TYPE_INT;
            }
            else if (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT)
            {
                expr_type = TYPE_FLOAT; // Added for float support
            }
            else if ((left_type == TYPE_INT && right_type == TYPE_FLOAT) || (left_type == TYPE_FLOAT && right_type == TYPE_INT))
            {
                expr_type = TYPE_FLOAT; // Type promotion for mixed int/float
            }
            else
            {
                sprintf(error_msg, "Arithmetic operands must be numeric (int/float) for operator %d.", op);
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        // Relational operations: <, <=, >, >=
        else if (op == TOKEN_LT || op == TOKEN_LEQ || op == TOKEN_GT || op == TOKEN_GEQ)
        {
            if ((left_type == TYPE_INT && right_type == TYPE_INT) ||
                (left_type == TYPE_FLOAT && right_type == TYPE_FLOAT) || // Added for float support
                (left_type == TYPE_CHAR && right_type == TYPE_CHAR)      // Can compare chars
            )
            {
                expr_type = TYPE_BOOLEAN;
            }
            else
            {
                sprintf(error_msg, "Relational operands must be compatible types for operator %d.", op);
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        // Equality operations: ==, !=
        else if (op == TOKEN_EQ || op == TOKEN_NEQ)
        {
            // Allow equality for primitive types if they are the same
            if (left_type == right_type &&
                (left_type == TYPE_INT || left_type == TYPE_BOOLEAN || left_type == TYPE_CHAR || left_type == TYPE_FLOAT || left_type == TYPE_STRING))
            {
                expr_type = TYPE_BOOLEAN;
            }
            else
            {
                sprintf(error_msg, "Equality operands must be of the same compatible primitive type for operator %d.", op);
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        // Logical operations: &&, ||
        else if (op == TOKEN_AND || op == TOKEN_OR)
        {
            if (left_type == TYPE_BOOLEAN && right_type == TYPE_BOOLEAN)
            {
                expr_type = TYPE_BOOLEAN;
            }
            else
            {
                sprintf(error_msg, "Logical operands must be booleans for operator %d.", op);
                report_semantic_error(error_msg, node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else
        {
            sprintf(error_msg, "Unsupported binary operator %d.", op);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        break;
    }

    case EXPR_UNARY:
    {
        TypeKind operand_type = analyze_expression_node(node->data.unary_expr.operand);
        if (operand_type == TYPE_ERROR)
        {
            expr_type = TYPE_ERROR;
            break;
        }

        int op = node->data.unary_expr.op;
        if (op == TOKEN_MINUS)
        {
            if (operand_type == TYPE_INT || operand_type == TYPE_FLOAT) // Allow unary minus on floats
            {
                expr_type = operand_type; // Type remains the same (int -> int, float -> float)
            }
            else
            {
                report_semantic_error("Operand for unary minus must be numeric (integer or float).", node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else if (op == TOKEN_NOT)
        {
            if (operand_type == TYPE_BOOLEAN)
            {
                expr_type = TYPE_BOOLEAN;
            }
            else
            {
                report_semantic_error("Operand for logical NOT '!' must be a boolean.", node->base.line_number);
                expr_type = TYPE_ERROR;
            }
        }
        else
        {
            sprintf(error_msg, "Unsupported unary operator %d.", op);
            report_semantic_error(error_msg, node->base.line_number);
            expr_type = TYPE_ERROR;
        }
        break;
    }

    case EXPR_PARENTHESIZED:
        if (!node->data.parenthesized_expr)
        {
            expr_type = TYPE_ERROR;
            break;
        }
        expr_type = analyze_expression_node(node->data.parenthesized_expr);
        break; // Exit PARENTHESIZED case

    case EXPR_TRUE:
    case EXPR_FALSE:
        expr_type = TYPE_BOOLEAN;
        break; // Exit BOOLEAN literal case

    default:
        report_semantic_error("Unknown expression kind encountered.", node->base.line_number);
        expr_type = TYPE_ERROR;
        break;
    }


    node->resolved_type = expr_type;

    return expr_type;
}