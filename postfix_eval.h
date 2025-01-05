#ifndef __POSTFIX_EVAL__
#define __POSTFIX_EVAL__

#include "tokens.h"

int postfix_eval(Token *expr, int token_count, double param, double *result);

#endif 
