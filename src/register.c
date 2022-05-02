#include "register.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "statement.h"
#include "utils.h"

extern char registers[8];
extern bool temporary[8];
extern struct Statement *current_function;

int find_empty_temporary_register() {
    for (int i = 0; i < 8; i++) {
        if (!temporary[i]) {
            temporary[i] = true;
            return i;
        }
    }
    return -1;
}

struct Register identifier_get_register(char *identifier_id) {
    int last = strlen(identifier_id) - 1;
    if (identifier_id[last] == ',')
        identifier_id[last] = '\0';

    int param_position = 0;
    // Checa se um identificador é um parâmetro de função.
    struct Expression *expression = current_function->function_metadata.parameters;
    while (expression) {
        if (strcmp(expression->identifier_metadata.name, identifier_id) == 0) {
            struct Register register_ = {.type = ARGUMENT, .position = param_position};
            return register_;
        }
        expression = expression->left;
        param_position++;
    }

    struct Register register_ = {.type = SAVED, .position = -1};
    for (int i = 0; i < 8; i++) {
        if (identifier_id[0] == registers[i]) {
            register_.position = i;
            return register_;
        }
    }
    return register_;
}
