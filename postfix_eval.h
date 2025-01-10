#ifndef __POSTFIX_EVAL__
#define __POSTFIX_EVAL__

#include "tokens.h"

/* evaluates postfix expression
 * stores result in *result
 * returns
 * 0 if successful, 1 otherwise*/
int postfix_eval(Token *expr, int token_count, double param, double *result);

#endif 
