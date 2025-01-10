#ifndef __POSTFIX_EVAL__
#define __POSTFIX_EVAL__

#include "tokens.h"

/**
 * @brief evaluates postfix expression
 *
 * @param expr expression to evaluate
 * @param token_count number of tokens
 * @param param parameter for the variable
 * @param result pointer for storing the result
 *
 * @return 1 if seccesfull
 */
int postfix_eval(Token *expr, int token_count, double param, double *result);

#endif 
