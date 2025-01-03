#ifndef __YARD__
#define __YARD__

#include "tokens.h"

int shunting_yard(Token **token_arr, int token_count);

int priority(Token *t);

#endif 
