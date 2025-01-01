#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"


int Check_if_limits(char *arg)
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

    limits = Check_if_limits(argv[argc - 1]);
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
    return 0;
}

