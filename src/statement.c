#include "statement.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "expression.h"
#include "list.h"
#include "parser.tab.h"
#include "register.h"
#include "utils.h"

char registers[8] = {0};
bool temporary[8] = {false};
int number_of_ifs = 0;
struct Statement *current_function = NULL;
int registers_in_use = 0;
bool is_current_function_main = false;

List split_format_string(char *source) {
    List strings = list_create(NULL, free);
    char *format_string = copy_string(source);
    char *original_pointer = format_string;

    // Remove aspas.
    format_string++;
    format_string[strlen(format_string) - 1] = '\0';

    char *substring = strtok(format_string, "%");
    while (substring) {
        // Remove símbolo de formatação
        if (substring != format_string)
            substring++;
        list_insert_tail(strings, copy_string(substring));
        substring = strtok(NULL, "%");
    }

    free(original_pointer);
    return strings;
}

void prepend_strings(struct Statement *statement) {
    static bool data_printed = false;
    static int str_count = 1;

    if (!data_printed) {
        printf(".data\n");
        data_printed = true;
    }

    while (statement != NULL) {
        if (statement->type == PRINTF) {
            List format_strings = split_format_string(statement->printf_metadata.format_string);
            for_each_list(no, format_strings) {
                char *string = list_get_info(no);
                printf("\ttext%d: .asciiz \"%s\"\n", str_count, string);
                str_count++;
            }
            list_destroy(format_strings);
        } else if (statement->type == IF) {
            prepend_strings(statement->conditional_metadata.if_);
            prepend_strings(statement->conditional_metadata.else_);
        }
        statement = statement->next;
    }
}

void call_function(struct Expression *expression) {
    const int registers_count = sizeof(registers) / sizeof(registers[0]);
    const int temp_registers_count = sizeof(temporary) / sizeof(temporary[0]);

    bool temp_registers_in_use[8] = {false};
    int temp_registers_in_use_count = 0;
    for (int i = 0; i < temp_registers_count; i++) {
        if (temporary[i]) {
            temp_registers_in_use[i] = true;
            temp_registers_in_use_count++;
        }
    }
    const int other_registers = 2;
    const int stack_alloc_size =
        4 * (registers_count + temp_registers_in_use_count + other_registers);

    printf("\t# FUNCTION (%s)\n", expression->function_metadata.name);
    // Aloca espaço na stack e salva os registradores.
    printf("\tsub $sp, $sp, %d\n", stack_alloc_size);
    printf("\tsw $ra, 0($sp)\n");
    printf("\tsw $a0, 4($sp)\n");
    for (int i = 0; i < 8; i++) {
        printf("\tsw $s%d, %d($sp)\n", i, (i + other_registers) * 4);
    }
    for (int i = 0; i < temp_registers_count; i++) {
        if (temp_registers_in_use[i]) {
            printf("\tsw $t%d, %d($sp)\n", i, 4 * (registers_count + i + other_registers));
        }
    }

    struct Expression *parameter = expression->function_metadata.parameters;
    while (parameter) {
        struct Register output = expression_evaluate(parameter);
        printf("\tadd $a0, $zero, $%c%d\n", output.type, output.position);
        parameter = parameter->next;
    }

    printf("\tjal %s\n", expression->function_metadata.name);

    for (int i = 7; i >= 0; i--) {
        if (temp_registers_in_use[i]) {
            printf("\tlw $t%d, %d($sp)\n", i, 4 * (registers_count + i + other_registers));
        }
    }
    for (int i = 7; i >= 0; i--) {
        printf("\tlw $s%d, %d($sp)\n", i, (i + other_registers) * 4);
    }
    printf("\tlw $a0, 4($sp)\n");
    printf("\tlw $ra, 0($sp)\n");
    printf("\taddi $sp, $sp, %d\n", stack_alloc_size);
}

void statement_evaluate(struct Statement *statement) {
    static int str_count = 1;
    static int if_count = 0;

    while (statement != NULL) {
        if (statement->type == FUNCTION) {
            printf("%s:\n", statement->function_metadata.name);
            is_current_function_main = strcmp(statement->function_metadata.name, "main") == 0;
            current_function = statement;

            struct Expression *exp = statement->function_metadata.parameters;
            while (exp) {
                const char *parameter = exp->identifier_metadata.name;
                LOG_INFO("Initializing parameter %s of function %s\n", parameter,
                         statement->function_metadata.name);
                registers[registers_in_use++] = parameter[0];
                exp = exp->left;
            }
        } else if (statement->type == IF) {
            if_count = ++number_of_ifs;
            printf("\t# IF CONDITION\n");

            struct Register expression_output =
                expression_evaluate(statement->conditional_metadata.condition);
            printf("\tbeq $%c%d, $zero, else_%d\n", expression_output.type,
                   expression_output.position, if_count);

            printf("# IF\n");
            statement_evaluate(statement->conditional_metadata.if_);
            printf("\tj endif_%d\n", if_count);

            printf("# ELSE\n");
            printf("else_%d:\n", if_count);
            statement_evaluate(statement->conditional_metadata.else_);

            printf("endif_%d:\n", if_count);
            if_count--;
        } else if (statement->type == PRINTF) {
            List format_strings = split_format_string(statement->printf_metadata.format_string);
            ListNode format_string_node = list_get_head(format_strings);

            printf("\t# PRINTF \"%s\"\n", (char *) list_get_info(format_string_node));
            printf("\taddi $v0, $zero, 4\n");
            printf("\tla $a0, text%d\n", str_count++);
            printf("\tsyscall\n");

            struct Expression *expression = statement->printf_metadata.expressions;
            while (expression != NULL) {
                if (expression->type == FUNCTION) {
                    // Caso uma função seja passada como parâmetro para o printf.
                    call_function(expression);
                    printf("\t# PRINTF function %s\n", expression->function_metadata.name);
                    printf("\tadd $a0, $zero, $v0\n");
                    printf("\taddi $v0, $zero, 1\n");
                    printf("\tsyscall\n");
                } else if (expression->type == IDENTIFIER) {
                    struct Register register_ =
                        identifier_get_register(expression->identifier_metadata.name);
                    printf("\t# PRINTF identifier %s\n", expression->identifier_metadata.name);
                    printf("\taddi $v0, $zero, 1\n");
                    printf("\tadd $a0, $zero, $%c%d\n", register_.type, register_.position);
                    printf("\tsyscall\n");
                }
                expression = expression->left;
                format_string_node = list_get_next(format_string_node);
                if (format_string_node) {
                    printf("\t# PRINTF \"%s\"\n", (char *) list_get_info(format_string_node));
                    printf("\taddi $v0, $zero, 4\n");
                    printf("\tla $a0, text%d\n", str_count);
                    printf("\tsyscall\n");
                    str_count++;
                }
            }
            list_destroy(format_strings);
        } else if (statement->type == SCANF) {
            printf("\t# SCANF\n");
            printf("\taddi $v0, $zero, 5\n");
            printf("\tsyscall\n");
            const int empty_register =
                identifier_get_register(statement->identifier_metadata.name).position;
            printf("\tadd $s%d, $v0, $zero\n", empty_register);
        } else if (statement->type == RETURN) {
            struct Register value = expression_evaluate(statement->return_metadata.expression);
            // Retorna ou sai do programa conforme necessário.
            if (is_current_function_main) {
                printf("\t# EXIT\n");
                printf("\taddi $v0, $zero, 17\n");
                printf("\tadd $a0, $zero, $%c%d\n", value.type, value.position);
                printf("\tsyscall\n");
            } else {
                printf("\t# RETURN\n");
                printf("\tadd $v0, $zero, $%c%d\n", value.type, value.position);
                printf("\tjr $ra\n");
            }
        } else if (statement->type == IDENTIFIER) {
            registers[registers_in_use] = statement->identifier_metadata.name[0];
            registers_in_use++;
        }
        statement = statement->next;
    }
}
