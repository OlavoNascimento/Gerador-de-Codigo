#pragma once

#include "parser.tab.h"

struct FunctionParameter {
    char *identifier;
    enum yytokentype type;
};

struct FunctionMetadata {
    char *name;
    enum yytokentype return_type;
    struct Expression *parameters;
};

struct IdentifierMetadata {
    char *name;
};

struct ConditionalMetadata {
    struct Expression *condition;
    struct Statement *if_;
    struct Statement *else_;
};

struct LoopMetadata {
    struct Expression *condition;
};

struct PrintfMetadata {
    char *format_string;
    struct Expression *expressions;
};

struct ReturnMetadata {
    struct Expression *expression;
};

struct NumberMetadata {
    int value;
};

struct Statement {
    enum yytokentype type;
    struct Statement *next;
    union {
        // Funções
        struct FunctionMetadata function_metadata;
        // Variável / Scanf
        struct IdentifierMetadata identifier_metadata;
        // If
        struct ConditionalMetadata conditional_metadata;
        // While
        struct LoopMetadata loop_metadata;
        // Printf
        struct PrintfMetadata printf_metadata;
        // Return
        struct ReturnMetadata return_metadata;
    };
};

struct Expression {
    enum yytokentype type;
    struct Expression *left;
    struct Expression *right;
    struct Expression *next;
    union {
        // Funções
        struct FunctionMetadata function_metadata;
        // Números
        struct NumberMetadata number_metadata;
        // Identificadores
        struct IdentifierMetadata identifier_metadata;
    };
};

struct Statement *ast_create_statement(enum yytokentype type, char *identifier_id, int return_type,
                                       struct Expression *parameters, struct Expression *exp,
                                       struct Statement *if_, struct Statement *else_,
                                       struct Statement *next);

struct Expression *ast_create_expression(enum yytokentype type, char *identifier_id, int value,
                                         struct Expression *parameters, struct Expression *left,
                                         struct Expression *right, struct Expression *next);

void ast_destroy_expression(struct Expression *expr);

void ast_destroy_statement(struct Statement *node);
