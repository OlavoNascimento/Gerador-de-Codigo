#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "parser.tab.h"
#include "utils.h"

struct Statement *ast_create_statement(enum yytokentype type, char *identifier_id, int return_type,
                                       struct Expression *parameters, struct Expression *exp,
                                       struct Statement *if_, struct Statement *else_,
                                       struct Statement *next) {
    struct Statement *cmd = malloc(sizeof *cmd);
    cmd->type = type;
    cmd->next = next;

    if (type == FUNCTION) {
        cmd->function_metadata.name = identifier_id;
        cmd->function_metadata.return_type = return_type;
        cmd->function_metadata.parameters = parameters;
    } else if (type == IF) {
        cmd->conditional_metadata.condition = exp;
        cmd->conditional_metadata.if_ = if_;
        cmd->conditional_metadata.else_ = else_;
    } else if (type == IDENTIFIER || type == SCANF) {
        cmd->identifier_metadata.name = identifier_id;
    } else if (type == PRINTF) {
        cmd->printf_metadata.format_string = identifier_id;
        cmd->printf_metadata.expressions = parameters;
    } else if (type == RETURN) {
        cmd->return_metadata.expression = exp;
    }
    return cmd;
}

struct Expression *ast_create_expression(enum yytokentype type, char *identifier_id, int value,
                                         struct Expression *parameters, struct Expression *left,
                                         struct Expression *right, struct Expression *next) {
    struct Expression *expression = malloc(sizeof *expression);
    expression->type = type;
    expression->left = left;
    expression->right = right;
    expression->next = next;
    if (type == FUNCTION) {
        expression->function_metadata.name = identifier_id;
        expression->function_metadata.parameters = parameters;
    } else if (type == NUMBER) {
        expression->number_metadata.value = value;
    } else if (type == IDENTIFIER || type == PARAMETER) {
        expression->identifier_metadata.name = identifier_id;
    }
    return expression;
}

void ast_destroy_statement(struct Statement *node) {
    if (!node)
        return;
    ast_destroy_statement(node->next);

    if (node->type == FUNCTION) {
        free(node->function_metadata.name);
        ast_destroy_expression(node->function_metadata.parameters);
    } else if (node->type == IF) {
        ast_destroy_expression(node->conditional_metadata.condition);
        ast_destroy_statement(node->conditional_metadata.if_);
        ast_destroy_statement(node->conditional_metadata.else_);
    } else if (node->type == PRINTF) {
        free(node->printf_metadata.format_string);
        ast_destroy_expression(node->printf_metadata.expressions);
    } else if (node->type == IDENTIFIER || node->type == SCANF) {
        free(node->identifier_metadata.name);
    } else if (node->type == RETURN) {
        ast_destroy_expression(node->return_metadata.expression);
        free(node);
    }
}

void ast_destroy_expression(struct Expression *expression) {
    if (!expression)
        return;
    if (expression->type == FUNCTION) {
        ast_destroy_expression(expression->function_metadata.parameters);
    } else if (expression->type == IDENTIFIER || expression->type == PARAMETER) {
        free(expression->identifier_metadata.name);
    }

    ast_destroy_expression(expression->left);
    ast_destroy_expression(expression->right);
    free(expression);
}
