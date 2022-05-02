#include "expression.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "statement.h"
#include "utils.h"

extern bool temporary[8];

struct Register evaluate_binary_operation(const char *asm_command, struct Expression *expression) {
    struct Register left_expression_output = expression_evaluate(expression->left);
    struct Register right_expression_output = expression_evaluate(expression->right);

    int output_register = -1;
    if (left_expression_output.type == SAVED && right_expression_output.type == SAVED) {
        output_register = find_empty_temporary_register();
        if (output_register == -1) {
            LOG_ERROR("No empty temporary register available\n");
        }
    } else if (left_expression_output.type == TEMP && right_expression_output.type == TEMP) {
        output_register = left_expression_output.position;
        temporary[right_expression_output.position] = 0;
    } else if (left_expression_output.type == TEMP) {
        output_register = left_expression_output.position;
    } else {
        output_register = right_expression_output.position;
    }

    printf("\t%s $t%d, $%c%d, $%c%d\n", asm_command, output_register, left_expression_output.type,
           left_expression_output.position, right_expression_output.type,
           right_expression_output.position);

    struct Register current_expression = {.type = TEMP, .position = output_register};

    return current_expression;
}

struct Register expression_evaluate(struct Expression *expression) {
    if (expression->type == EQUAL) {
        printf("\t# EQUAL\n");
        return evaluate_binary_operation("seq", expression);
    } else if (expression->type == FUNCTION) {
        call_function(expression);
        const int empty_register = find_empty_temporary_register();
        printf("\tadd $t%d, $zero, $v0\n", empty_register);
        struct Register register_ = {.position = empty_register, .type = TEMP};
        return register_;
    } else if (expression->type == LESS_EQUAL) {
        printf("\t# LESS_EQUAL\n");
        return evaluate_binary_operation("sle", expression);
    } else if (expression->type == LESS_THAN) {
        printf("\t# LESS_THAN\n");
        return evaluate_binary_operation("slt", expression);
    } else if (expression->type == LOGICAL_AND) {
        printf("\t# LOGICAL_AND\n");
        return evaluate_binary_operation("and", expression);
    } else if (expression->type == LOGICAL_OR) {
        printf("\t# LOGICAL_OR\n");
        return evaluate_binary_operation("or", expression);
    } else if (expression->type == MINUS) {
        printf("\t# SUB\n");
        return evaluate_binary_operation("sub", expression);
    } else if (expression->type == MULTIPLY) {
        printf("\t# MULTIPLY\n");
        return evaluate_binary_operation("mul", expression);
    } else if (expression->type == NUMBER) {
        printf("\t# Number\n");
        const int empty_register = find_empty_temporary_register();
        printf("\taddi $t%d, $zero, %d\n", empty_register, expression->number_metadata.value);
        struct Register register_ = {.position = empty_register, .type = TEMP};
        return register_;
    } else if (expression->type == PLUS) {
        printf("\t# ADD\n");
        return evaluate_binary_operation("add", expression);
    } else if (expression->type == IDENTIFIER) {
        return identifier_get_register(expression->identifier_metadata.name);
    } else {
        LOG_ERROR("Unknown value expression type: %d\n", expression->type);
    }
    struct Register register_ = {.position = -1, .type = TEMP};
    return register_;
}
