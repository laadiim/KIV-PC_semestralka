#ifndef __YARD__
#define __YARD__

#include "tokens.h"

/* shunting yard algorithm
 * returns 
 * count of tokens if successful, 0 otherwise
 * result in *token_arr */
int shunting_yard(Token **token_arr, int token_count);

/* returns priority of given token, 0 if unrecognised */
int priority(Token *t);

#endif 
