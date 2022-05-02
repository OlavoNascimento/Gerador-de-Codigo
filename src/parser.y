%{
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>

    #include "parser.tab.h"
    #include "statement.h"
    #include "utils.h"

    int yylex();
    void yyerror(const char *);
    struct Statement *move_main_function_to_top(struct Statement *ast_root);

    struct Statement *statement;
%}

%define parse.error verbose

%union {
    char *text;
    int number;
    struct Expression *expression_node;
    struct Statement * statement_node;
}

%token AST_DEFINITION

%token END_FUNCTION
%token FUNCTION
%token PARAMETER
%token RETURN
%token RETURN_TYPE
%token VARIABLE

%token CHAR
%token INT
%token TYPE
%token VOID

%token COMMA
%token IF
%token L_PAREN
%token PRINTF
%token R_PAREN
%token SCANF
%token SEMI_COLON

%token <text> IDENTIFIER
%token <text> STRING
%token <number> NUMBER

%token AND
%token EQUAL
%token GREATER_EQUAL
%token GREATER_THAN
%token LESS_EQUAL
%token LESS_THAN
%token LOGICAL_AND
%token LOGICAL_OR
%token MINUS
%token MULTIPLY
%token PLUS

%type <text> identifier string
%type <number> binary_operator function_return type
%type <statement_node> commands function if instructions possible_function printf return scanf
%type <expression_node> expression function_call parameters parameters_function_call printf_parameters value

%start start

%%

start: AST_DEFINITION function {
        statement = move_main_function_to_top($2);
    }
;

function: FUNCTION identifier RETURN_TYPE function_return parameters instructions END_FUNCTION possible_function {
        $$ = ast_create_statement(FUNCTION, $2, $4, $5, NULL, NULL, NULL, $6);

        // Conecta uma função com a próxima função.
        struct Statement *last_statement = $$;
        while(true) {
            if(!last_statement->next)
                break;
            last_statement = last_statement->next;
        }
        last_statement->next = $8;
    }
;

possible_function: function {
        $$ = $1;
    }
    | {
        $$ = NULL;
    }
;

identifier: IDENTIFIER {
        $$ = copy_string($1);
    }
;

string: STRING {
        $$ = copy_string($1);
    }
;

parameters: PARAMETER identifier TYPE type parameters {
        $$ = ast_create_expression(PARAMETER, $2, 0, NULL, NULL, $5, NULL);
    }
    | {
        $$ = NULL;
    }
;

instructions: commands SEMI_COLON instructions {
        $1->next = $3;
        $$ = $1;
    }
    | commands {
        $$ = $1;
    }
;

commands: printf {
        $$ = $1;
    }
    | scanf	{
        $$ = $1;
    }
    | if {
        $$ = $1;
    }
    | return {
        $$ = $1;
    }
    | VARIABLE identifier TYPE type instructions {
        $$ = ast_create_statement(IDENTIFIER, $2, $4, NULL, NULL, NULL, NULL, $5);
    }
;

return: RETURN L_PAREN NUMBER R_PAREN {
        struct Expression *number = ast_create_expression(NUMBER, NULL, $3, NULL, NULL, NULL, NULL);
        $$ = ast_create_statement(RETURN, NULL, 0, NULL, number, NULL, NULL, NULL);
    }
    | RETURN L_PAREN expression R_PAREN {
        $$ = ast_create_statement(RETURN, NULL, 0, NULL, $3, NULL, NULL, NULL);
}
;

type: INT {
        $$ = INT;
    }
    | CHAR {
        $$ = CHAR;
    }
;

function_return: VOID {
        $$ = VOID;
    }
    | type {
        $$ = $1;
    }
;

printf: PRINTF L_PAREN string R_PAREN {
        $$ = ast_create_statement(PRINTF, $3, -1, NULL, NULL, NULL, NULL, NULL);
    }
    | PRINTF L_PAREN string COMMA printf_parameters R_PAREN {
        $$ = ast_create_statement(PRINTF, $3, -1, $5, NULL, NULL, NULL, NULL);
    }
;

printf_parameters: identifier {
        struct Expression *exp_node = ast_create_expression(IDENTIFIER, $1, 0, NULL, NULL, NULL, NULL);
        $$ = exp_node;
    }
    | identifier COMMA printf_parameters {
        struct Expression *exp_node = ast_create_expression(IDENTIFIER, $1, 0, NULL, NULL, NULL, NULL);
        exp_node->left = $3;
        $$ = exp_node;
    }
    | function_call {
        $$ = $1;
    }
    | function_call COMMA printf_parameters {
        $1->left = $3;
        $$ = $1;
    }
;

scanf: SCANF L_PAREN string COMMA AND L_PAREN identifier R_PAREN R_PAREN {
        $$ = ast_create_statement(SCANF, $7, 0, NULL, NULL, NULL, NULL, NULL);
        free($3);
    }
;

if: IF L_PAREN expression COMMA commands COMMA commands R_PAREN {
        $$ = ast_create_statement(IF, NULL, 0, NULL, $3, $5, $7, NULL);
    }
    | IF L_PAREN expression COMMA commands R_PAREN {
        $$ = ast_create_statement(IF, NULL, 0, NULL, $3, $5, NULL, NULL);
    }
;

expression: binary_operator L_PAREN value COMMA value R_PAREN {
        $$ = ast_create_expression($1, NULL, 0, NULL, $3, $5, NULL);
    }
;

function_call: identifier L_PAREN parameters_function_call R_PAREN {
        int last = strlen($1) - 1;
        if($1[last] == '(' )
            $1[last] = '\0';
        $$ = ast_create_expression(FUNCTION, $1, 0, $3, NULL, NULL, NULL);
};

parameters_function_call: value COMMA parameters_function_call {
        $1->next = $3;
        $$ = $1;
    }
    | value {
        $$ = $1;
    }
    | expression COMMA parameters_function_call {
        $1->next = $3;
        $$ = $1;
    }
    | expression {
        $$ = $1;
    }
;

binary_operator: PLUS {
        $$ = PLUS;
    }
    | MULTIPLY {
        $$ = MULTIPLY;
    }
    | MINUS {
        $$ = MINUS;
    }
    | EQUAL {
        $$ = EQUAL;
    }
    | LESS_THAN {
        $$ = LESS_THAN;
    }
    | LESS_EQUAL {
        $$ = LESS_EQUAL;
    }
    | GREATER_THAN {
        $$ = GREATER_THAN;
    }
    | GREATER_EQUAL {
        $$ = GREATER_EQUAL;
    }
    | LOGICAL_AND {
        $$ = LOGICAL_AND;
    }
    | LOGICAL_OR {
        $$ = LOGICAL_OR;
    }
;

value: expression {
        $$ = $1;
    }
    | NUMBER {
        $$ = ast_create_expression(NUMBER, NULL, $1, NULL, NULL, NULL, NULL);
    }
    | IDENTIFIER {
        $$ = ast_create_expression(IDENTIFIER, copy_string($1), 0, NULL, NULL, NULL, NULL);
    }
    | function_call {
        $$ = $1;
    }
;

%%

// Move a função main para o início do arquivo para não gerar erros ao executar o programa no Mars-Mips.
struct Statement *move_main_function_to_top(struct Statement *ast_root) {
    struct Statement *main_function = ast_root;
    struct Statement *last_statement_before_main = main_function;
    // Procura a função main e o último statement antes da definição da main na AST.
    while (strcmp(main_function->function_metadata.name, "main") != 0) {
        last_statement_before_main = main_function;
        main_function = main_function->next;
    }

    // Procura o último comando executado na função main.
    struct Statement *last_statement_in_main = last_statement_before_main->next->next;
    while (true) {
        if (!last_statement_in_main->next && last_statement_in_main->type != FUNCTION)
            break;
        last_statement_in_main = last_statement_in_main->next;
    }

    // Move a função main para o início do arquivo apenas quando ela já não é a primeira função.
    if (last_statement_before_main != main_function) {
        last_statement_before_main->next = last_statement_in_main->next;
        last_statement_in_main->next = ast_root;
    }
    return main_function;
}

void yyerror (const char *err) {
    printf("ERROR: %s\n", err);
    exit(1);
}
