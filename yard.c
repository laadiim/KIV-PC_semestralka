#include <stdio.h>
#include <stdlib.h>

#include "tokens.h"
#include "stack.h"
#include "yard.h"

int priority(Token *t)
{
	/* sanity check */
	if (t == NULL) return 0;

	if (t->type == TOKEN_LPARENTHESIS || t->type == TOKEN_RPARENTHESIS) return 7;

	if (t->type == TOKEN_FUNCTION) return 6;

	if (t->type == TOKEN_UNARY_OPERATOR) return 5;

	if (t->type == TOKEN_NUMBER || t->type == TOKEN_VARIABLE) return 1;

	if (t->type == TOKEN_BINARY_OPERATOR)
	{
		if (t->value[0] == '^') return 4;
		if (t->value[0] == '*' || t->value[0] == '/') return 3;
		if (t->value[0] == '+' || t->value[0] == '-') return 2;
	}
	return 0;
}

int shunting_yard(Expression *exp)
{
	int i, postfix_ptr = 0, p;
	int e = 0;
	stack *token_stack = NULL;
	Token **postfix = NULL;
	Token *tmp = NULL;
	int token_count = 0;
	Token **token_arr = NULL;

	/* sanity check */
	if (exp == NULL) return 0;

	token_count = exp->len;
	token_arr = exp->arr;

	postfix = (Token **)(malloc(sizeof(Token *) * token_count));
	if (postfix == NULL) return 0;

	token_stack = stack_create((unsigned int)token_count);
	if (token_stack == NULL)
	{
		free(postfix);
		postfix = NULL;
		return 0;
	}

	for (i = 0; i < token_count; i++)
	{
		p = priority(token_arr[i]);
		if (!p) e = 1;

		if (p == 1)
		{
			postfix[postfix_ptr] = token_arr[i];
			postfix_ptr++;
		}

		if (token_arr[i]->type == TOKEN_LPARENTHESIS) stack_push(token_stack, token_arr[i]);

		if (token_arr[i]->type == TOKEN_RPARENTHESIS) 
		{
			tmp = stack_pop(token_stack);
			while (tmp != NULL && tmp->type != TOKEN_LPARENTHESIS)
			{
				postfix[postfix_ptr] = tmp;
				postfix_ptr++;
				tmp = stack_pop(token_stack);
			}
		}
		if (token_arr[i]->type == TOKEN_BINARY_OPERATOR || token_arr[i]->type == TOKEN_UNARY_OPERATOR || token_arr[i]->type == TOKEN_FUNCTION)
		{
			tmp = stack_peek(token_stack);
			while (tmp != NULL && priority(tmp) >= p && priority(tmp) <= 6)
			{
				postfix[postfix_ptr] = stack_pop(token_stack);
				postfix_ptr++;
				tmp = stack_peek(token_stack);
			}
			stack_push(token_stack, token_arr[i]);
		}
	}

	tmp = stack_pop(token_stack);
	while (tmp != NULL)
	{
		postfix[postfix_ptr] = tmp;
		postfix_ptr++;
		tmp = stack_pop(token_stack);
	}

	stack_clear(&token_stack);

	if (e) return 0;

	exp->len = postfix_ptr;
	exp->arr = postfix;
	return postfix_ptr;
}
