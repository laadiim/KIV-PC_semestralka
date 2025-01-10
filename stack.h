#ifndef __STACK__
#define __STACK__
#include "tokens.h"

typedef struct 
{
    Token **items;
    int sp;
    unsigned int size;
} stack;

stack *stack_create(unsigned int size);

int stack_push(stack *s, Token *item);

Token *stack_pop(stack *s);

Token *stack_peek(stack *s);

void stack_clear(stack **s);

#endif
