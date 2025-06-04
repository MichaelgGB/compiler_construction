#include "ir_generator.h"
#include "tac.h"
#include "symbol_table.h" // For lookup_symbol and current_scope
#include "ast.h"          // For AST node structures
#include "miniJava.tab.h" // For TOKEN_ definitions (ensure this path is correct)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for internal helper functions
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

    if (!program_root->class_decl)
    {
        fprintf(stderr, "IR Gen Error: Program has no class declaration.\n");
        return;
    }

    init_tac_generator(); // Initialize TAC list and counters

    ClassDeclarationNode *class_decl = program_root->class_decl;
    Scope *original_scope_before_ir = current_scope;

    if (class_decl->associated_scope)
    {
        current_scope = class_decl->associated_scope;
    }
    else
    {
        fprintf(stderr, "IR Gen Warning: Class '%s' has no associated scope. Assuming global scope.\n",
                class_decl->name ? class_decl->name->name : "UnnamedClass");
        // If no class scope, operations might resolve to global or fail if they expect class members.
    }

    if (class_decl->main_method)
    {
        MainMethodNode *main_method = class_decl->main_method;
        Scope *scope_before_main = current_scope; // This would be the class scope (or global if class had no scope)

        TacOperand main_label_op = create_tac_label_operand(main_method->name ? main_method->name->name : "main");
        // Use TAC_FUNCTION_BEGIN for clarity, or TAC_LABEL if preferred
        emit_tac(TAC_FUNCTION_BEGIN, create_tac_none_operand(), main_label_op, create_tac_none_operand(), main_method->base.line_number);
        // Note: create_tac_label_operand strdup's the name, so if main_label_op is not used elsewhere,
        // its name would be freed by free_tac_code. If main_label_op itself needs freeing, do it after all uses.

        if (main_method->associated_scope)
        {
            current_scope = main_method->associated_scope;
        }
        else
        {
            fprintf(stderr, "IR Gen Error: Main method in class '%s' has no associated scope. Using enclosing scope.\n",
                    class_decl->name ? class_decl->name->name : "UnnamedClass");
            // IR generation will use scope_before_main for lookups if main has no distinct scope.
        }

        if (main_method->body)
        {
            generate_ir_for_block_statement(main_method->body);
        }

        // Emit an explicit return for main if not already present, or handle in TAC_FUNCTION_END
        // emit_tac(TAC_RETURN, create_tac_none_operand(), create_tac_none_operand(), create_tac_none_operand(), main_method->base.line_number);
        emit_tac(TAC_FUNCTION_END, create_tac_none_operand(), main_label_op, create_tac_none_operand(), main_method->base.line_number);

        current_scope = scope_before_main; // Restore to class scope (or whatever was before main)
    }
    else
    {
        fprintf(stderr, "IR Gen Error: No main method found in class '%s'.\n",
                class_decl->name ? class_decl->name->name : "UnnamedClass");
    }

    current_scope = original_scope_before_ir; // Restore original scope
}

TacOperand generate_ir_for_expression(ExpressionNode *expr_node)
{
    TacOperand result_op = create_tac_none_operand();

    if (!expr_node)
    {
        fprintf(stderr, "IR Gen Error: NULL expression node encountered.\n");
        return result_op; // Return a 'none' operand to signify error or absence
    }

    // Use the resolved_type from semantic analysis for new temporaries
    TypeKind current_expr_type = expr_node->resolved_type;
    if (current_expr_type == TYPE_ERROR || current_expr_type == TYPE_UNDEFINED)
    {
        // fprintf(stderr, "IR Gen Warning: Generating IR for expression at line %d with unresolved or error type.\n", expr_node->base.line_number);
        // Fallback or default type for temporaries if needed, or propagate error type
        current_expr_type = TYPE_INT; // Example fallback, ideally propagate TYPE_ERROR
    }

    switch (expr_node->kind)
    {
    case EXPR_LITERAL:
    {
        LiteralNode *lit = expr_node->data.literal_expr;
        if (!lit)
        {
            fprintf(stderr, "IR Gen Error: NULL literal in expression at line %d.\n", expr_node->base.line_number);
            return create_tac_none_operand();
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
            fprintf(stderr, "IR Gen Error: Unknown literal kind %d at line %d.\n", lit->kind, lit->base.line_number);
            break;
        }
        break;
    }
    case EXPR_BOOLEAN_TRUE:
        result_op = create_tac_bool_literal_operand(1);
        break;
    case EXPR_BOOLEAN_FALSE:
        result_op = create_tac_bool_literal_operand(0);
        break;
    case EXPR_IDENTIFIER:
    {
        IdentifierNode *id_node = expr_node->data.identifier_expr;
        if (!id_node || !id_node->name)
        {
            fprintf(stderr, "IR Gen Error: Malformed identifier expression at line %d.\n", expr_node->base.line_number);
            return create_tac_none_operand();
        }
        SymbolTableEntry *entry = lookup_symbol(id_node->name);
        if (!entry)
        {
            fprintf(stderr, "IR Gen Error: Identifier '%s' not found (line %d). Semantic analysis should catch this.\n", id_node->name, expr_node->base.line_number);
            // To prevent crashes, return a dummy or error operand.
            // This indicates a severe issue if semantic analysis passed.
            return create_tac_var_operand(id_node->name, TYPE_ERROR); // Mark as error type
        }
        result_op = create_tac_var_operand(entry->name, entry->type);
        break;
    }
    case EXPR_BINARY:
    {
        if (!expr_node->data.binary_expr.left || !expr_node->data.binary_expr.right)
        {
            fprintf(stderr, "IR Gen Error: Malformed binary expression at line %d.\n", expr_node->base.line_number);
            return create_tac_temp_operand(TYPE_ERROR);
        }
        TacOperand left_op = generate_ir_for_expression(expr_node->data.binary_expr.left);
        TacOperand right_op = generate_ir_for_expression(expr_node->data.binary_expr.right);
        TacOperand temp_res = create_tac_temp_operand(current_expr_type); // Use resolved type
        TacOpCode tac_op;

        switch (expr_node->data.binary_expr.op_token)
        {
        case TOKEN_PLUS:
            tac_op = TAC_ADD;
            break;
        case TOKEN_MINUS:
            tac_op = TAC_SUB;
            break;
        case TOKEN_MULTIPLY:
            tac_op = TAC_MUL;
            break;
        case TOKEN_DIVIDE:
            tac_op = TAC_DIV;
            break;
        case TOKEN_MOD:
            tac_op = TAC_MOD;
            break;
        case TOKEN_LT:
            tac_op = TAC_LT;
            break;
        case TOKEN_GT:
            tac_op = TAC_GT;
            break;
        case TOKEN_LEQ:
            tac_op = TAC_LEQ;
            break;
        case TOKEN_GEQ:
            tac_op = TAC_GEQ;
            break;
        case TOKEN_EQ:
            tac_op = TAC_EQ;
            break;
        case TOKEN_NEQ:
            tac_op = TAC_NEQ;
            break;
        case TOKEN_AND:
            tac_op = TAC_AND;
            break;
        case TOKEN_OR:
            tac_op = TAC_OR;
            break;
        default:
            fprintf(stderr, "IR Gen Error: Unknown binary operator token %d at line %d.\n", expr_node->data.binary_expr.op_token, expr_node->base.line_number);
            return temp_res; // Return the temp, but it won't have a valid generating instruction
        }
        emit_tac(tac_op, temp_res, left_op, right_op, expr_node->base.line_number); // Pass line number
        result_op = temp_res;
        break;
    }
    case EXPR_UNARY:
    {
        if (!expr_node->data.unary_expr.operand)
        {
            fprintf(stderr, "IR Gen Error: Malformed unary expression at line %d.\n", expr_node->base.line_number);
            return create_tac_temp_operand(TYPE_ERROR);
        }
        TacOperand operand_op = generate_ir_for_expression(expr_node->data.unary_expr.operand);
        TacOperand temp_res = create_tac_temp_operand(current_expr_type); // Use resolved type
        TacOpCode tac_op;
        switch (expr_node->data.unary_expr.op_token)
        {
        case TOKEN_MINUS:
            tac_op = TAC_UMINUS;
            break;
        case TOKEN_NOT:
            tac_op = TAC_NOT;
            break;
        default:
            fprintf(stderr, "IR Gen Error: Unknown unary operator token %d at line %d.\n", expr_node->data.unary_expr.op_token, expr_node->base.line_number);
            return temp_res;
        }
        emit_tac(tac_op, temp_res, operand_op, create_tac_none_operand(), expr_node->base.line_number); // Pass line
        result_op = temp_res;
        break;
    }
    case EXPR_PARENTHESIZED:
    {
        if (!expr_node->data.parenthesized_expr_val)
        {
            fprintf(stderr, "IR Gen Error: Malformed parenthesized expression at line %d.\n", expr_node->base.line_number);
            return create_tac_none_operand();
        }
        result_op = generate_ir_for_expression(expr_node->data.parenthesized_expr_val);
        break;
    }
    case EXPR_ARRAY_ACCESS:
    {
        IdentifierNode *array_id_node = expr_node->data.array_access_expr.array_name_ident;
        ExpressionNode *index_expr_node = expr_node->data.array_access_expr.index_expr;

        if (!array_id_node || !array_id_node->name || !index_expr_node)
        {
            fprintf(stderr, "IR Gen Error: Malformed array access at line %d.\n", expr_node->base.line_number);
            return create_tac_temp_operand(TYPE_ERROR);
        }

        SymbolTableEntry *array_entry = lookup_symbol(array_id_node->name);
        if (!array_entry || array_entry->type != TYPE_INT_ARRAY)
        {
            fprintf(stderr, "IR Gen Error: Identifier '%s' is not a recognized array type or not declared (line %d).\n", array_id_node->name, expr_node->base.line_number);
            return create_tac_temp_operand(TYPE_ERROR);
        }

        TacOperand array_op = create_tac_var_operand(array_id_node->name, array_entry->type);
        TacOperand index_op = generate_ir_for_expression(index_expr_node);

        result_op = create_tac_temp_operand(current_expr_type);                               // Element type
        emit_tac(TAC_ARRAY_LOAD, result_op, array_op, index_op, expr_node->base.line_number); // Pass line
        break;
    }
    default:
        fprintf(stderr, "IR Gen Error: Unhandled ExpressionKind %d (AstNodeType %d) at line %d.\n",
                expr_node->kind, expr_node->base.node_type, expr_node->base.line_number);
        return create_tac_none_operand(); // Or a specific error operand
    }
    return result_op;
}

void generate_ir_for_statement(StatementNode *stmt_node)
{
    if (!stmt_node)
    {
        fprintf(stderr, "IR Gen Error: NULL statement node encountered.\n");
        return;
    }

    switch (stmt_node->base.node_type)
    {
    case NODE_STATEMENT_VAR_DECL_WRAPPER:
        if (stmt_node->data.var_decl_data)
            generate_ir_for_variable_declaration(stmt_node->data.var_decl_data);
        break;
    case NODE_STATEMENT_ASSIGNMENT_WRAPPER:
        if (stmt_node->data.assignment_data)
            generate_ir_for_assignment(stmt_node->data.assignment_data);
        break;
    case NODE_STATEMENT_IF_WRAPPER:
        if (stmt_node->data.if_data)
            generate_ir_for_if_statement(stmt_node->data.if_data);
        break;
    case NODE_STATEMENT_WHILE_WRAPPER:
        if (stmt_node->data.while_data)
            generate_ir_for_while_statement(stmt_node->data.while_data);
        break;
    case NODE_STATEMENT_PRINT_WRAPPER:
        if (stmt_node->data.print_data)
            generate_ir_for_print_statement(stmt_node->data.print_data);
        break;
    case NODE_STATEMENT_RETURN_WRAPPER:
        if (stmt_node->data.return_data)
            generate_ir_for_return_statement(stmt_node->data.return_data);
        break;
    case NODE_STATEMENT_BLOCK_WRAPPER:
        if (stmt_node->data.block_data)
            generate_ir_for_block_statement(stmt_node->data.block_data);
        break;
    case NODE_STATEMENT_EXPRESSION_WRAPPER:
        if (stmt_node->data.expression_data)
        {
            generate_ir_for_expression(stmt_node->data.expression_data); // Result is discarded
        }
        break;
    default:
        fprintf(stderr, "IR Gen Error: Unhandled statement AstNodeType %d (kind %d) at line %d.\n",
                stmt_node->base.node_type, stmt_node->kind, stmt_node->base.line_number);
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

void generate_ir_for_variable_declaration(VariableDeclarationNode *var_decl_node)
{
    if (!var_decl_node || !var_decl_node->identifier || !var_decl_node->identifier->name || !var_decl_node->var_type)
    {
        fprintf(stderr, "IR Gen Error: Malformed variable declaration node at line %d.\n", var_decl_node ? var_decl_node->base.line_number : -1);
        return;
    }

    if (var_decl_node->initializer)
    {
        SymbolTableEntry *entry = lookup_symbol(var_decl_node->identifier->name);
        if (!entry)
        { // Should be caught by semantic analysis
            fprintf(stderr, "IR Gen Error: Variable '%s' (line %d) not found in symbol table during IR gen for initializer.\n",
                    var_decl_node->identifier->name, var_decl_node->base.line_number);
            return;
        }
        TacOperand var_op = create_tac_var_operand(entry->name, entry->type);
        TacOperand initializer_op = generate_ir_for_expression(var_decl_node->initializer);
        emit_tac(TAC_ASSIGN, var_op, initializer_op, create_tac_none_operand(), var_decl_node->base.line_number);
    }
}

void generate_ir_for_assignment(AssignmentNode *assign_node)
{
    if (!assign_node || !assign_node->target_identifier || !assign_node->target_identifier->name || !assign_node->value)
    {
        fprintf(stderr, "IR Gen Error: Malformed assignment node at line %d.\n", assign_node ? assign_node->base.line_number : -1);
        return;
    }

    TacOperand value_op = generate_ir_for_expression(assign_node->value);

    if (assign_node->array_index)
    {
        IdentifierNode *array_id_node = assign_node->target_identifier;
        ExpressionNode *index_expr_node = assign_node->array_index;

        if (!array_id_node->name || !index_expr_node)
        { // Added null check for name
            fprintf(stderr, "IR Gen Error: Malformed array assignment (missing array name or index) at line %d.\n", assign_node->base.line_number);
            return;
        }
        SymbolTableEntry *array_entry = lookup_symbol(array_id_node->name);
        if (!array_entry || (array_entry->type != TYPE_INT_ARRAY /* && other array types */))
        {
            fprintf(stderr, "IR Gen Error: Identifier '%s' for array assignment is not an array or not declared (line %d).\n",
                    array_id_node->name, assign_node->base.line_number);
            return;
        }

        TacOperand array_op = create_tac_var_operand(array_id_node->name, array_entry->type);
        TacOperand index_op = generate_ir_for_expression(index_expr_node);
        // For TAC_ARRAY_STORE: result=array_base, op1=index, op2=value_to_store
        emit_tac(TAC_ARRAY_STORE, array_op, index_op, value_op, assign_node->base.line_number);
    }
    else
    {
        SymbolTableEntry *entry = lookup_symbol(assign_node->target_identifier->name);
        if (!entry)
        { // Should be caught by semantic analysis
            fprintf(stderr, "IR Gen Error: Target identifier '%s' not found for assignment (line %d).\n",
                    assign_node->target_identifier->name, assign_node->base.line_number);
            return;
        }
        TacOperand target_op = create_tac_var_operand(entry->name, entry->type);
        emit_tac(TAC_ASSIGN, target_op, value_op, create_tac_none_operand(), assign_node->base.line_number);
    }
}

void generate_ir_for_if_statement(IfStatementNode *if_stmt_node)
{
    if (!if_stmt_node || !if_stmt_node->condition || !if_stmt_node->then_branch)
    {
        fprintf(stderr, "IR Gen Error: Malformed if statement node at line %d.\n", if_stmt_node ? if_stmt_node->base.line_number : -1);
        return;
    }

    TacOperand cond_op = generate_ir_for_expression(if_stmt_node->condition);

    char *else_label_str = generate_new_label_name();
    TacOperand else_label = create_tac_label_operand(else_label_str); // else_label_str is now owned by else_label.val.name

    char *end_if_label_str = NULL;
    TacOperand end_if_label = create_tac_none_operand();

    if (if_stmt_node->else_branch)
    {
        end_if_label_str = generate_new_label_name();
        end_if_label = create_tac_label_operand(end_if_label_str); // end_if_label_str owned by end_if_label.val.name
    }

    // For TAC_IF_NOT_GOTO: result=target_label, op1=condition
    emit_tac(TAC_IF_NOT_GOTO, else_label, cond_op, create_tac_none_operand(), if_stmt_node->condition->base.line_number);

    generate_ir_for_statement(if_stmt_node->then_branch);

    if (if_stmt_node->else_branch)
    {
        // For TAC_GOTO: result=target_label
        emit_tac(TAC_GOTO, end_if_label, create_tac_none_operand(), create_tac_none_operand(), if_stmt_node->then_branch->base.line_number); // Use a line from then_branch
    }

    // For TAC_LABEL: result=label_name
    emit_tac(TAC_LABEL, else_label, create_tac_none_operand(), create_tac_none_operand(), if_stmt_node->base.line_number); // Line of the if/else transition
    // Do NOT free else_label_str here, its memory is managed by the TacOperand's val.name
    // It will be freed by free_tac_code().

    if (if_stmt_node->else_branch)
    {
        generate_ir_for_statement(if_stmt_node->else_branch);
        emit_tac(TAC_LABEL, end_if_label, create_tac_none_operand(), create_tac_none_operand(), if_stmt_node->else_branch->base.line_number);
        // Do NOT free end_if_label_str here.
    }
}

void generate_ir_for_while_statement(WhileStatementNode *while_stmt_node)
{
    if (!while_stmt_node || !while_stmt_node->condition || !while_stmt_node->body)
    {
        fprintf(stderr, "IR Gen Error: Malformed while statement node at line %d.\n", while_stmt_node ? while_stmt_node->base.line_number : -1);
        return;
    }

    char *loop_start_label_str = generate_new_label_name();
    TacOperand loop_start_label = create_tac_label_operand(loop_start_label_str);

    char *loop_end_label_str = generate_new_label_name();
    TacOperand loop_end_label = create_tac_label_operand(loop_end_label_str);

    // For TAC_LABEL: result=label_name
    emit_tac(TAC_LABEL, loop_start_label, create_tac_none_operand(), create_tac_none_operand(), while_stmt_node->base.line_number);
    // Do NOT free loop_start_label_str.

    TacOperand cond_op = generate_ir_for_expression(while_stmt_node->condition);
    // For TAC_IF_NOT_GOTO: result=target_label, op1=condition
    emit_tac(TAC_IF_NOT_GOTO, loop_end_label, cond_op, create_tac_none_operand(), while_stmt_node->condition->base.line_number);

    generate_ir_for_statement(while_stmt_node->body);

    // For TAC_GOTO: result=target_label
    emit_tac(TAC_GOTO, loop_start_label, create_tac_none_operand(), create_tac_none_operand(), while_stmt_node->body->base.line_number);

    emit_tac(TAC_LABEL, loop_end_label, create_tac_none_operand(), create_tac_none_operand(), while_stmt_node->base.line_number); // Line for end of loop
    // Do NOT free loop_end_label_str.
}

void generate_ir_for_print_statement(PrintStatementNode *print_stmt_node)
{
    if (!print_stmt_node || !print_stmt_node->expression)
    {
        fprintf(stderr, "IR Gen Error: Malformed print statement node at line %d.\n", print_stmt_node ? print_stmt_node->base.line_number : -1);
        return;
    }
    TacOperand expr_op = generate_ir_for_expression(print_stmt_node->expression);

    // CORRECTED: For TAC_PRINT, op1 is the value to print. result and op2 are none.
    emit_tac(TAC_PRINT,
             create_tac_none_operand(),          // result
             expr_op,                            // op1 (value to print)
             create_tac_none_operand(),          // op2
             print_stmt_node->base.line_number); // source line number
}

void generate_ir_for_return_statement(ReturnStatementNode *return_stmt_node)
{
    if (!return_stmt_node)
    {
        fprintf(stderr, "IR Gen Error: Malformed return statement node (NULL).\n");
        return;
    }

    if (return_stmt_node->expression)
    {
        TacOperand expr_op = generate_ir_for_expression(return_stmt_node->expression);
        // For TAC_RETURN: op1 is the value to return. result and op2 are none.
        emit_tac(TAC_RETURN,
                 create_tac_none_operand(), // result
                 expr_op,                   // op1
                 create_tac_none_operand(), // op2
                 return_stmt_node->base.line_number);
    }
    else
    {
        emit_tac(TAC_RETURN,
                 create_tac_none_operand(),
                 create_tac_none_operand(), // op1 is none for void return
                 create_tac_none_operand(),
                 return_stmt_node->base.line_number);
    }
}

void generate_ir_for_block_statement(BlockStatementNode *block_stmt_node)
{
    if (!block_stmt_node)
    {
        fprintf(stderr, "IR Gen Error: Malformed block statement node (NULL).\n");
        return;
    }

    Scope *scope_before_block = current_scope;

    if (block_stmt_node->associated_scope)
    {
        current_scope = block_stmt_node->associated_scope;
    }
    else
    {
        fprintf(stderr, "IR Gen Warning: Block statement at line %d has no associated scope. Using enclosing scope.\n",
                block_stmt_node->base.line_number);
    }

    generate_ir_for_statement_list(block_stmt_node->statements);

    current_scope = scope_before_block;
}