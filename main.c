#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "stack.h"
#include "yard.h"
#include "postfix_eval.h"
#include "postscript.h"


#define STEPS 2000

/* checks if function is mathematically correct
 * 1 if passed, 0 if failed*/
int check_if_correct(Token *expr[], int token_count)
{
	stack *token_stack = NULL;
	int i = 0;
	Token *tmp = NULL;
	int error = 0;

	/* sanity check */
	if (expr == NULL || token_count == 0) return 0;

	token_stack = stack_create(token_count);
	if (token_stack == NULL) return 0;

	if ((*expr)[0].type == TOKEN_BINARY_OPERATOR || (*expr)[0].type == TOKEN_RPARENTHESIS) error = 1;

	for (i = 0; i < token_count - 1; i++)
	{
		/* left bracket found -> push to stack */
		if ((*expr)[i].type == TOKEN_LPARENTHESIS)
		{
			if (!stack_push(token_stack, &(*expr)[i])) error = 1;
			if ((*expr)[i + 1].type == TOKEN_BINARY_OPERATOR || (*expr)[i + 1].type == TOKEN_RPARENTHESIS) error = 1;
		}

		/* right bracket -> pop from stack */
		if ((*expr)[i].type == TOKEN_RPARENTHESIS)
		{
			tmp = stack_pop(token_stack);
			if (tmp == NULL)
			{
				printf("Closing brackets too soon\n");
				error = 1;
			}
			if ((*expr)[i + 1].type != TOKEN_BINARY_OPERATOR) error = 1;
		}

		/* else check rules */
		if ((*expr)[i].type == TOKEN_BINARY_OPERATOR || (*expr)[i].type == TOKEN_UNARY_OPERATOR)
		{
			if ((*expr)[i + 1].type == TOKEN_BINARY_OPERATOR || (*expr)[i + 1].type == TOKEN_RPARENTHESIS) error = 1;
		}

		if ((*expr)[i].type == TOKEN_FUNCTION && (*expr)[i + 1].type != TOKEN_LPARENTHESIS) error = 1;

		if (((*expr)[i].type == TOKEN_NUMBER || (*expr)[i].type == TOKEN_VARIABLE || (*expr)[i].type == TOKEN_RPARENTHESIS) && ((*expr)[i + 1].type != TOKEN_BINARY_OPERATOR && (*expr)[i + 1].type != TOKEN_RPARENTHESIS)) error = 1;

	}

	if ((*expr)[token_count - 1].type == TOKEN_RPARENTHESIS)
	{
		tmp = stack_pop(token_stack);
		if (tmp == NULL)
		{
			printf("Closing brackets too soon\n");
			error = 1;
		}
	}
	if ((*expr)[token_count - 1].type == TOKEN_BINARY_OPERATOR || (*expr)[token_count - 1].type == TOKEN_FUNCTION || (*expr)[token_count - 1].type == TOKEN_LPARENTHESIS) error = 1;

	if (stack_peek(token_stack) != NULL)
	{
		printf("Unclosed bracket\n");
		error = 1;
	}
	stack_clear(&token_stack);
	return !error;
}

/* check for limits
 * number of ':' or 1 if invalid arguments*/
int check_limits(char *arg)
{
	int counter = 0;
	
	/* sanity check */
	if (arg == NULL) return 1;

	while (*arg != '\0')
	{
		if (*arg == ':') counter++;
		arg++;
	}
	return counter;
}


/* get limit values 
 * return 0 if failed, 1 if successful*/
int get_limits(char *arg, double *x_min, double *x_max, double *y_min, double *y_max)
{
	char *endptr = NULL;

	/* sanity check */
	if (arg == NULL || x_min == NULL || x_max == NULL || y_min == NULL || y_max == NULL) return 0;

	*x_min = strtod(arg, &endptr);
	if (*endptr != ':') return 0;
	arg = endptr + 1;
	*x_max = strtod(arg, &endptr);
	if (*endptr != ':') return 0;
	arg = endptr + 1;
	*y_min = strtod(arg, &endptr);
	if (*endptr != ':') return 0;
	arg = endptr + 1;
	*y_max = strtod(arg, &endptr);
	if (*endptr != '\0') return 0;
	return 1;
}

/* ENTRY POINT */
int main(int argc, char *argv[])
{
    int limits = 0;
		char *out_name = NULL;
		Token **token_arr = NULL;
		int token_count = 0;
		int i = 0;

		double x_min = -10;
		double x_max = 10;
		double y_min = -10;
		double y_max = 10;

		double step = 0;
		double x_values[STEPS + 1];
		double y_values[STEPS + 1];

		int func_error = 0;

		setlocale(LC_NUMERIC, "C");

    /* Sanity check */
    if (argc < 3)
    {
        printf("Not enough parameters!\n");
        printf("Usage: %s <equation> <output file> [<limits>]\n", argv[0]);
        return 1;
    }


		/* get limits */
		limits = check_limits(argv[argc - 1]);
		if (limits == 3)
		{
			limits = get_limits(argv[argc - 1], &x_min, &x_max, &y_min, &y_max);
			if (limits == 0)
			{
				printf("Invalid limits format!\n");
				printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
				return 4;
			}
			if (x_min >= x_max || y_min >= y_max)
			{
				printf("Invalid limits values!\n");
				printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
				return 4;
			}
		}

		out_name = argv[argc - 1 - limits];

		/* tokenize */
		token_arr = (Token **)(malloc(sizeof(Token *)));
		token_count = tokenize(argv, argc, limits, token_arr);
		printf("%d\n", token_count);
		/* print tokens */
		if (token_count <= 0)
		{
			printf("Function was not in the correct format!\n");
			free(token_arr);
			token_arr = NULL;
			return 2;
		}

		/* check correctness */
		if(!check_if_correct(token_arr, token_count))
		{
			printf("Function was not in mathematically correct format!\n");
			free(token_arr);
			token_arr = NULL;
			return 2;
		}

		/* transform to postfix */
		token_count = shunting_yard(token_arr, token_count);
		
		if (token_count <= 0)
		{
			printf("Function was not in the correct format!\n");
			free(token_arr);
			token_arr = NULL;
			return 2;
		}

		/* evaluate for required numbers */
		step = (x_max - x_min) / (STEPS);

		for (i = 0; i < STEPS + 1; i++)
		{
			if (!postfix_eval(*token_arr, token_count, x_min + i * step, &(y_values[i])))
			{
				printf("Function could not be evaluated!\n");
				func_error = 1;
				break;
			}
			x_values[i] = x_min + i * step;
			printf("%f : %f\n", x_values[i], y_values[i]);

		}

		if (func_error)
		{
			printf("Function was not in the correct format!\n");
			free(token_arr);
			token_arr = NULL;
			return 2;
		}
		
		/* create output file */
		create_postscript(x_values, y_values, STEPS + 1, x_min, x_max, y_min, y_max, out_name);

    printf("limits: %f:%f:%f:%f\n", x_min, x_max, y_min, y_max);
		printf("out file: %s\n", out_name);
		free(*token_arr);
		free(token_arr);
    return 0;
}

