#ifndef __YARD__
#define __YARD__

#include "tokens.h"

/* returns priority of given token, 0 if unrecognised */
int priority(Token *t);

int shunting_yard(Expression *exp);

#endif 
