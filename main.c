#include <string.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"
#include "stack.h"
#include "yard.h"

int check_if_correct(Token *expr[], int token_count)
{
	stack *token_stack;
	int i;
	Token *tmp;
	int error = 0;

	token_stack = stack_create(token_count);

	for (i = 0; i < token_count - 1; i++)
	{
		if ((*expr)[i].type == TOKEN_LPARENTHESIS)
		{
			printf("Push\n");
			stack_push(token_stack, &(*expr)[i]);
			if ((*expr)[i + 1].type == TOKEN_BINARY_OPERATOR || (*expr)[i + 1].type == TOKEN_RPARENTHESIS) error = 1;
		}

		if ((*expr)[i].type == TOKEN_RPARENTHESIS)
		{
			printf("Pop\n");
			tmp = stack_pop(token_stack);
			if (tmp == NULL)
			{
				printf("Closing too much\n");
				error = 1;
			}
			if ((*expr)[i + 1].type != TOKEN_BINARY_OPERATOR) error = 1;
		}

		if ((*expr)[i].type == TOKEN_BINARY_OPERATOR || (*expr)[i].type == TOKEN_UNARY_OPERATOR)
		{
			if ((*expr)[i + 1].type == TOKEN_BINARY_OPERATOR || (*expr)[i + 1].type == TOKEN_RPARENTHESIS) error = 1;
		}

		if ((*expr)[i].type == TOKEN_FUNCTION && (*expr)[i + 1].type != TOKEN_LPARENTHESIS) error = 1;

		if (((*expr)[i].type == TOKEN_NUMBER || (*expr)[i].type == TOKEN_VARIABLE || (*expr)[i].type == TOKEN_RPARENTHESIS) && (*expr)[i + 1].type != TOKEN_BINARY_OPERATOR) error = 1;

	}
	if (stack_peek(token_stack) != NULL)
	{
		printf("Unclosed bracket\n");
		error = 1;
	}
	stack_clear(&token_stack);
	return !error;
}

int check_if_limits(char *arg)
{
    int colons = 0;
    while (*arg != '\0')
    {
        if (*arg == ':') colons++;
        arg += sizeof(char);
    }
    return colons;
}

/* ENTRY POINT */
int main(int argc, char *argv[])
{
    int limits = 0;
		char *out_name;
		Token **token_arr;
		int token_count;
		int i;

		setlocale(LC_NUMERIC, "C");

    /* Sanity check */
    if (argc < 3)
    {
        printf("Not enough parameters!\n");
        printf("Usage: %s <equation> <output file> [<limits>]\n", argv[0]);
        return 1;
    }

    limits = check_if_limits(argv[argc - 1]);
		if (limits != 0 && limits != 3)
		{
			printf("Invalid limits format!\n");
      printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
			return 4;
		}

		out_name = argv[argc - 1 - limits / 3];

    printf("limits: %d\n", limits);
		printf("out file: %s\n", out_name);

		token_count = tokenize(argv, argc, limits / 3, token_arr);
		printf("%d\n", token_count);
		/* print tokens */
		if (token_count > 0)
		{
			for (i = 0; i < token_count; i++)
			{
				printf("%d: %s\n", (*token_arr)[i].type,(*token_arr)[i].value);
			}
		}

		printf("Checking parenthesis: %d\n", check_if_correct(token_arr, token_count));

		token_count = shunting_yard(token_arr, token_count);
		for (i = 0; i < token_count; i++)
		{
			printf("%s", (*token_arr)[i].value);
		}
		printf("\n");

    return 0;
}

