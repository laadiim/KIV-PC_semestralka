#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "tokens.h"
#include "postfix_eval.h"
#include "stack.h"

#define NOTANUMBER 100001
#define MAX 100000
#define MIN -100000

#define FREE_ALL {\
				stack_clear(&token_stack);\
				free(tokens_copy);\
				tokens_copy = NULL;\
				return  0;\
				}

typedef enum {
	F_UNDEFINED,
	SIN,
	COS,
	TAN,
	SINH,
	COSH,
	TANH,
	ASIN,
	ACOS,
	ATAN,
	ABS,
	LOG,
	LN,
	EXP
} functions;

typedef enum {
	O_UNDEFINED,
	ADD,
	SUB,
	MUL,
	DIV,
	POW
} operators;

int get_function(Token *func)
{
	
	if (!strcmp(func->value, "sin")) return SIN;
	if (!strcmp(func->value, "cos")) return COS;
	if (!strcmp(func->value, "tan")) return TAN;
	if (!strcmp(func->value, "sinh")) return SINH;
	if (!strcmp(func->value, "cosh")) return COSH;
	if (!strcmp(func->value, "tanh")) return TANH;
	if (!strcmp(func->value, "asin")) return ASIN;
	if (!strcmp(func->value, "acos")) return ACOS;
	if (!strcmp(func->value, "atan")) return ATAN;
	if (!strcmp(func->value, "abs")) return ABS;
	if (!strcmp(func->value, "log")) return LOG;
	if (!strcmp(func->value, "ln")) return LN;
	if (!strcmp(func->value, "exp")) return EXP;
	return F_UNDEFINED;
}

int get_operator(Token *op)
{
	if (!strcmp(op->value, "+")) return ADD;
	if (!strcmp(op->value, "-")) return SUB;
	if (!strcmp(op->value, "*")) return MUL;
	if (!strcmp(op->value, "/")) return DIV;
	if (!strcmp(op->value, "^")) return POW;
	return O_UNDEFINED;
}

int eval_function(Token *func, Token *parameter, Token **result)
{
	int f = 0;
	double param = 0;
	double r = 0;
	char *endptr = NULL;
	if (result == NULL || func == NULL || parameter == NULL) return 0;

	param = strtod(parameter->value, &endptr);
	if (*endptr != '\0') return 0;

	*result = (Token *)(malloc(sizeof(Token)));
	if (*result == NULL) return 0;

	f = get_function(func);
	switch (f) {
		case SIN:
			r = sin(param);
			break;
		case COS:
			r = cos(param);
			break;
		case TAN:
			r = tan(param);
			break;
		case SINH:
			r = sinh(param);
			break;
		case COSH:
			r = cosh(param);
			break;
		case TANH:
			r = tanh(param);
			break;
		case ASIN:
			if (param < -1 || param > 1) r = NOTANUMBER;
			else r = asin(param);
			break;
		case ACOS:
			if (param < -1 || param > 1) r = NOTANUMBER;
			else r = acos(param);
			break;
		case ATAN:
			r = atan(param);
			break;
		case ABS:
			r = fabs(param);
			break;
		case LOG:
			r = param <= 0 ? NOTANUMBER : log10(param);
			break;
		case LN:
			r = param <= 0 ? NOTANUMBER : log(param);
			break;
		case EXP:
			r = exp(param);
			break;
		default:
			r = NOTANUMBER;
			break;
	}

	if (r != NOTANUMBER)	r = r > MAX ? MAX : (r < MIN ? MIN : r);

	sprintf((*result)->value, "%f", r);
	(*result)->type = TOKEN_NUMBER;
	return 1;
}

int eval_operator(Token *op, Token *num1, Token *num2, Token **result)
{
	int o = 0;
	double n1 = 0;
	double n2 = 0;
	double r = 0;
	char *endptr = NULL;

	if (op == NULL || num1 == NULL || num2 == NULL || result == NULL) return 0;

	n1 = strtod(num1->value, &endptr);
	if (*endptr != '\0') return 0;
	n2 = strtod(num2->value, &endptr);
	if (*endptr != '\0') return 0;

	*result = (Token *)(malloc(sizeof(Token)));
	if (*result == NULL) return 0;

	o = get_operator(op);
	switch (o)
	{
		case ADD:
			r = n1 + n2;
			break;
		case SUB: 
			r = n1 - n2;
			break;
		case MUL:
			r = n1 * n2;
			break;
		case DIV:
			r = n2 == 0 ? NOTANUMBER : n1 / n2;
			break;
		case POW:
			r = pow(n1, n2);
			break;
		default:
			r = NOTANUMBER;
			break;
	}
	
	if (r != NOTANUMBER) r = r > MAX ? MAX : (r < MIN ? MIN : r);

	sprintf((*result)->value, "%f", r);
	(*result)->type = TOKEN_NUMBER;
	return 1;
}

int postfix_eval(Token *expr, int token_count, double param, double *result)
{
	stack *token_stack = NULL;
	Token *tokens_copy = NULL;
	int i = 0;
	Token *curr, *tmp1, *tmp2;
	double tmp = 0;
	char *endptr = NULL;

	if (expr == NULL || result == NULL || token_count < 1) return 0;

	token_stack = stack_create(token_count);
	if (token_stack == NULL) return 0;
	
	tokens_copy = (Token *)(malloc(token_count * sizeof(Token)));
	if (tokens_copy == NULL)
	{
		stack_clear(&token_stack);
		return 0;
	}

	memcpy(tokens_copy, expr, token_count * sizeof(Token));

	for (i = 0; i < token_count; i++)
	{
		curr = &(tokens_copy[i]);
		if (curr->type == TOKEN_NUMBER) 
		{
			if(!stack_push(token_stack, curr)) FREE_ALL; 
		}

		if (curr->type == TOKEN_VARIABLE)
		{
			curr->type = TOKEN_NUMBER;
			sprintf(curr->value, "%f", param);
			if(!stack_push(token_stack, curr)) FREE_ALL;
		}

		if (curr->type == TOKEN_UNARY_OPERATOR)
		{
			curr = stack_peek(token_stack);
			if (curr == NULL) FREE_ALL;
			tmp = strtod(curr->value, &endptr);
			if (*endptr != '\0') FREE_ALL;
			sprintf(curr->value, "%f", -tmp);
		}

		if (curr->type == TOKEN_FUNCTION)
		{
			tmp1 = stack_pop(token_stack);
			if (tmp1 == NULL) FREE_ALL;
			if (!eval_function(curr, tmp1, &tmp2)) FREE_ALL;
			if (!stack_push(token_stack, tmp2)) FREE_ALL;
		}

		if (curr->type == TOKEN_BINARY_OPERATOR)
		{
			tmp1 = stack_pop(token_stack);
			tmp2 = stack_pop(token_stack);
			if (tmp2 == NULL) FREE_ALL;
			if (!eval_operator(curr, tmp2, tmp1, &curr)) FREE_ALL;
			if (!stack_push(token_stack, curr)) FREE_ALL;
		}
	}
	curr = stack_pop(token_stack);
	if (curr == NULL) FREE_ALL;
	*result = strtod(curr->value, &endptr);
	if (*endptr != '\0') FREE_ALL;
	curr = stack_pop(token_stack);
	if (curr != NULL) FREE_ALL;
	free(tokens_copy);
	tokens_copy = NULL;
	stack_clear(&token_stack);
	return 1;
}
