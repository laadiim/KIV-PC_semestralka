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

#define FREE free(*token_arr);\
	*token_arr = NULL;\
	free(token_arr);\
	token_arr = NULL;

/**
 * @brief checks if expression is mathematically correct
 *
 * @param expr[]	expression to evaluate
 * @param token_count		number of tokens
 *
 * @return is correct
 */
int check_if_correct(Token *expr[], int token_count)
{
	stack *token_stack = NULL;
	int i = 0;
	Token *tmp = NULL;
	int error = 0;

	/* sanity check */
	if (expr == NULL || token_count == 0) return 0;

	token_stack = stack_create(token_count);
	/* sanity check - stack creation */
	if (token_stack == NULL) return 0;

	/* starting token cant be binary operator or right bracket */
	if ((*expr)[0].type == TOKEN_BINARY_OPERATOR || (*expr)[0].type == TOKEN_RPARENTHESIS) error = 1;

	/* check all tokens except for last */
	for (i = 0; i < token_count - 1; i++)
	{
		/* left bracket found -> push to stack */
		if ((*expr)[i].type == TOKEN_LPARENTHESIS)
		{
			if (!stack_push(token_stack, &(*expr)[i])) error = 1;
			
			/* if left bracket followed by binary operator '(+' or right bracket '()', expression is incorrect */
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

			/* if right bracket not followed by binary operator */
			if ((*expr)[i + 1].type != TOKEN_BINARY_OPERATOR) error = 1;
		}

		/* rules for both operators */
		if ((*expr)[i].type == TOKEN_BINARY_OPERATOR || (*expr)[i].type == TOKEN_UNARY_OPERATOR)
		{
			/* if following token is another binary operator o right bracket, expression is incorrect */
			if ((*expr)[i + 1].type == TOKEN_BINARY_OPERATOR || (*expr)[i + 1].type == TOKEN_RPARENTHESIS) error = 1;
		}

		/* if function is not followed by left bracket, expression is incorrect */
		if ((*expr)[i].type == TOKEN_FUNCTION && (*expr)[i + 1].type != TOKEN_LPARENTHESIS) error = 1;

		/* if number, variable or right bracket is not followed by binary operator or right parenthesis, expression is incorrect */
		if (((*expr)[i].type == TOKEN_NUMBER || (*expr)[i].type == TOKEN_VARIABLE || (*expr)[i].type == TOKEN_RPARENTHESIS) && ((*expr)[i + 1].type != TOKEN_BINARY_OPERATOR && (*expr)[i + 1].type != TOKEN_RPARENTHESIS)) error = 1;

	}

	/* last tokent is right bracket -> pop from stack */
	if ((*expr)[token_count - 1].type == TOKEN_RPARENTHESIS)
	{
		tmp = stack_pop(token_stack);
		if (tmp == NULL)
		{
			printf("Closing brackets too soon\n");
			error = 1;
		}
	}

	/* last operator cant be binary operator, function or left bracket */
	if ((*expr)[token_count - 1].type == TOKEN_BINARY_OPERATOR || (*expr)[token_count - 1].type == TOKEN_FUNCTION || (*expr)[token_count - 1].type == TOKEN_LPARENTHESIS) error = 1;

	/* stack should be empty by the end */
	if (stack_peek(token_stack) != NULL)
	{
		printf("Unclosed bracket\n");
		error = 1;
	}
	stack_clear(&token_stack);
	return !error;
}

/**
 * @brief checks if limits are present in argument
 *
 * @param arg argument to check
 *
 * @return number of ':'
 */
int check_limits(char *arg)
{
	int counter = 0;
	
	/* sanity check */
	if (arg == NULL) return 1;

	/* count ':' */
	while (*arg != '\0')
	{
		if (*arg == ':') counter++;
		arg++;
	}

	return counter;
}


/**
 * @brief gets limits from the argument and stores them at the corresponding adress
 *
 * @param arg argument to process
 * @param x_min where to store x_min
 * @param x_max where to store x_max
 * @param y_min where to store y_min
 * @param y_max where to store y_max
 *
 * @return 1 if everything was good, 0 if an error occured
 */
int get_limits(char *arg, double *x_min, double *x_max, double *y_min, double *y_max)
{
	char *endptr = NULL;

	/* sanity check */
	if (arg == NULL || x_min == NULL || x_max == NULL || y_min == NULL || y_max == NULL) return 0;

	/* checks if number is followed by ':' */
	*x_min = strtod(arg, &endptr);
	if (*endptr != ':') return 0;
	arg = endptr + 1;

	*x_max = strtod(arg, &endptr);
	if (*endptr != ':') return 0;
	arg = endptr + 1;

	*y_min = strtod(arg, &endptr);
	if (*endptr != ':') return 0;
	arg = endptr + 1;

	/* last number must be followed by '\0' - end */
	*y_max = strtod(arg, &endptr);
	if (*endptr != '\0') return 0;
	
	return 1;
}

/**
 * @brief Main function
 *
 * @param argc number of arguments
 * @param argv[] array of arguments
 *
 * @return 
 */
int main(int argc, char *argv[])
{
		char *out_name = NULL; /* name for the output file */
		Token **token_arr = NULL; 
		int token_count = 0; /* number of tokens */
		int i = 0;

		/* limits */
		double x_min = -10;
		double x_max = 10;
		double y_min = -10;
		double y_max = 10;

		/* values for drawing */
		double step = 0;
		double x_values[STEPS];
		double y_values[STEPS];

		int func_error = 0; /* flag for error occuring while processing function */
    int limits = 0; /* flag for existence of limits */

		/* need to set locale - later changing all commas for dots for decimals */
		setlocale(LC_NUMERIC, "C");

    /* Sanity check */
    if (argc < 3)
    {
        printf("Not enough parameters!\n");
        printf("Usage: %s <equation> <output file> [<limits>]\n", argv[0]);
        return 1;
    }

		/* parsing limits */
		limits = check_limits(argv[argc - 1]);
		/* limits are now count of ':' */
		if (!limits)
		{
			printf("No limits found.");
		}
		else if (limits == 3)
		{
			limits = get_limits(argv[argc - 1], &x_min, &x_max, &y_min, &y_max);
			/* check if limits are in correct format */
			if (limits == 0)
			{
				printf("Invalid limits format!\n");
				printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
				return 4;
			}

			/* check if limit values make sense */
			if (x_min >= x_max || y_min >= y_max)
			{
				printf("Invalid limits values!\n");
				printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
				return 4;
			}
		}
		else 
		{
				printf("Invalid limits format!\n");
				printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
				return 4;
		}

		/* get output file name */
		out_name = argv[argc - 1 - limits];

		/* tokenize */
		token_arr = (Token **)(malloc(sizeof(Token *)));
		token_count = tokenize(argv, argc, limits, token_arr);
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
		step = (x_max - x_min) / (STEPS - 1);

		for (i = 0; i < STEPS; i++)
		{
			x_values[i] = x_min + i * step;
			if (!postfix_eval(*token_arr, token_count, x_values[i], &(y_values[i])))
			{
				printf("Function could not be evaluated for parameter x = %.4f!\n", x_values[i]);
				func_error = 1;
				break;
			}

			x_values[i] = (x_values[i] - x_min) * (GRAPH_WIDTH / (x_max - x_min)) + OFFSET;
			y_values[i] = (y_values[i] - y_min) * (GRAPH_HEIGHT / (y_max - y_min)) + OFFSET;
		}

		/* if error was detected, do not draw graph, instead throw an error */
		if (func_error)
		{
			printf("Function was not in the correct format!\n");
			free(token_arr);
			token_arr = NULL;
			return 2;
		}
		
		/* create output file */
		if (!create_postscript(x_values, y_values, STEPS, x_min, x_max, y_min, y_max, out_name))
		{
			printf("Output file %s could not be created!", out_name);
			free(*token_arr);
			free(token_arr);
		}

		free(*token_arr);
		free(token_arr);
    return 0;
}

