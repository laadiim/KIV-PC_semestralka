#include "tokens.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define VAR 'x'
#define OPS "+-*/^()"
#define FUNCTIONS                                                              \
  {"sin",  "cos",  "tan", "asin", "acos", "atan", "sinh",                      \
   "cosh", "tanh", "abs", "exp",  "ln",   "log"}

int remove_spaces(char *str) {
  int i = 0, j = 0;

  /* sanity check */
  if (str == NULL)
    return 1;

  while (str[i] != '\0') /* Traverse the string */
  {
    if (str[i] == ',')
      str[i] = '.';

    if (str[i] != ' ') /* If not a space */
    {
      str[j] = str[i]; /* Copy character to new position */
      j++;
    }
    i++;
  }

  str[j] = '\0'; /* Null-terminate the string */
  return 0;
}

int prepare(char **expr, int size, int limits) {
  int len = 0;
  int i;
  for (i = 1; i < size - limits - 1; i++) {
    len += strlen(expr[i]);
  }
  char concat[len];
  for (i = 1; i < size - limits - 1; i++) {
    strcat(concat, expr[i]);
  }
  printf("%s\n", concat);

  remove_spaces(concat);
  len = strlen(concat);

  printf("%s\n", concat);

  for (i = 0; i < len; i++) {
    printf("%d\n", concat[i]);
  }

  return 0;
}

int is_var(char *p) { return *p == VAR; }

int is_operator(char *p) {
  int i;
  char *ops = OPS;
  for (i = 0; i < strlen(ops); i++) {
    if (*p == ops[i]) {
      return 1;
    }
  }
  return 0;
}

int is_unary_minus(char *p, Token *last) {
	if (last->type == TOKEN_VARIABLE || last->type == TOKEN_NUMBER) return 0;
	if (last->type == TOKEN_BINARY_OPERATOR && strcmp(last->value, ")") == 0) return 0;
	return 1;
}

int is_number(char *str) {
  const char *p = str;  /* Pointer to traverse the string */
  int has_digits = 0;   /* Flag to ensure we encounter at least one digit */
  int has_exponent = 0; /* Flag for exponent (E or e) presence */
  int has_dot = 0;      /* Flag for decimal point presence */
  int exponent_digits = 0; /* Flag for exponent digits */

  /* Parse the main part of the number */
  while (isdigit(*p)) {
    has_digits = 1; /* Found at least one digit */
    p++;
  }

  /* Check for a decimal point */
  if (*p == '.') {
    has_dot = 1;
    p++;
    /* Parse digits after the decimal point */
    while (isdigit(*p)) {
      has_digits = 1; /* Found at least one digit */
      p++;
    }
  }

  /* Check for an exponent (E or e) */
  if ((*p == 'E' || *p == 'e') && has_digits) {
    has_exponent = 1;
    p++;

    /* Check for an optional sign after the exponent */
    if (*p == '+' || *p == '-') {
      p++;
    }

    while (isdigit(*p)) {
      exponent_digits = 1; /* Found at least one digit in the exponent */
      p++;
    }

    /* Exponent is invalid if it has no digits */
    if (!exponent_digits) {
      return 0;
    }
  }

  /* If we encountered no digits at all, it's invalid */
  if (!has_digits) {
    return 0;
  }

  /* Return the length of the valid number substring */
  return p - str;
}


int is_function(char *str) {
  int i;
  const char *functions[] = FUNCTIONS;
  // Loop through the array of function names
  for (i = 0; i < sizeof(functions) / sizeof(functions[0]); i++) {
    if (strcmp(str, functions[i]) == 0) {
      return strlen(
          functions[i]); // Return 1 if the string matches a function name
    }
  }
  return 0; // Return 0 if no match is found
}

int tokenize(char *expr[], int size, int limits, Token **result) {
  char *p;
  int unknown = 0;
	return 0;
}
