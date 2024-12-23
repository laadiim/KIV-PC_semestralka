#include "tokens.h"
#include <stdio.h>
#include <string.h>

void remove_spaces(char *str)
{
    int i = 0, j = 0;

    while (str[i] != '\0')  // Traverse the string
    {
        if (str[i] != ' ')  // If not a space
        {
            str[j] = str[i];  // Copy character to new position
            j++;
        }
        i++;
    }

    str[j] = '\0';  // Null-terminate the string
}

char **tokenize(char *expr[], int size, int limits)
{
	int len = 0;
	int i;
	for (i = 1; i < size - limits - 1; i++)
	{
		len += strlen(expr[i]);
	}
	char concat[len];
	for (i = 1; i < size - limits - 1; i++)
	{
		strcat(concat, expr[i]);
	}
	printf("%s\n", concat);

	remove_spaces(concat);
	len = strlen(concat);

	printf("%s\n", concat);

	for (i = 0; i < len; i++)
	{
		printf("%d\n", concat[i]);
	}

  return &concat;
}
