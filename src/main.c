#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "statement.h"

extern struct Statement *statement;

int main() {
    yyparse();
    prepend_strings(statement);
    printf(".text\n");
    statement_evaluate(statement);
    ast_destroy_statement(statement);
    return 0;
}
