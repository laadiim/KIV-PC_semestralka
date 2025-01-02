#ifndef __STACK__
#define __STACK__
#include "tokens.h"
#define uint unsigned int

typedef struct 
{
    Token **items;
    int sp;
    uint size;
} stack;

stack *stack_create(uint size);

int stack_push(stack *s, Token *item);

Token *stack_pop(stack *s);

Token *stack_peek(stack *s);

void stack_clear(stack **s);

#endif
