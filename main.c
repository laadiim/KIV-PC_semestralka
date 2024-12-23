#include <stdio.h>
#include <stdlib.h>


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

    /* Sanity check */
    if (argc < 2)
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
    return EXIT_SUCCESS;
}

