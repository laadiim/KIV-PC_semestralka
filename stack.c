#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "stack.h"


stack *stack_create(uint size)
{
    stack *temp;

    /* sanity check */
    if (!size) return NULL;

    /* allocate the stack */
    temp = (stack *) malloc(sizeof(stack));
    if (!temp) return NULL;

    temp->sp = -1;
    temp->size = size;
    temp->items = (Token **) malloc(size * sizeof(Token*));
    if (!temp->items) 
    {
        free(temp);
        return NULL;
    }

    return temp;
}

int stack_push(stack *s, Token *item)
{
    /* sanity check */
    if (!s || !item) return 0;
    if (s->sp >= (int)s->size - 1) {
        printf("Stack overflow detected\n");
        return 0;
    }

    /* store the value into the stack */
    s->sp++;
		s->items[s->sp] = item;
    return 1;
}

Token *stack_pop(stack *s)
{
    /* sanity check */
    if (!s) return NULL;
    if (s->sp < 0) {
        printf("Stack underflow detected\n");
        return NULL;
    }

    /* retrieve the popped value*/

    s->sp--;

    return s->items[s->sp + 1];
}

Token *stack_peek(stack *s)
{
    /* sanity check */
    if (!s) return NULL;
    if (s->sp < 0) {
        printf("Stack is empty\n");
        return NULL;
    }

    /* retrieve the popped value*/
    return s->items[s->sp];
}

void stack_clear(stack **s)
{
    /* sanity check */
    if (!s ||!*s) return;

    /* deallocate the stack */
    free((*s)->items);
    free(*s);
    *s = NULL;
}
