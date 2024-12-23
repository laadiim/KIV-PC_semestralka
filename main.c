#include <stdio.h>
#include <stdlib.h>

/* ENTRY POINT */
int main(int argc, char *argv[])
{
	/* sanity check */
	if (argc < 2)
	{
		printf("Not enough parameters!\n");
		printf("Usage: %s <equation> <x_min>:<x_max>:<y_min>:<y_max>", argv[0]);
		return 1;
	}
	return EXIT_SUCCESS;
}
