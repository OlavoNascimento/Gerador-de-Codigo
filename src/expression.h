#pragma once

#include "ast.h"
#include "register.h"

struct Register expression_evaluate(struct Expression *expression);
