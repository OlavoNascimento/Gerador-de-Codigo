#pragma once

#include "ast.h"

void call_function(struct Expression *expression);

void prepend_strings(struct Statement *statement);

void statement_evaluate(struct Statement *statement);
