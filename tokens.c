#include "tokens.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 256
#define VAR 'x'
#define OPS "+-*/^"
#define FUNCTIONS {"asin", "acos", "atan", "sinh", "cosh", "tanh", "sin",  "cos",  "tan",  "abs", "exp",  "log", "ln"}

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

char *prepare(char **expr, int size, int limits) {
  int len = 0;
  int i = 0;
  char *concat = NULL;

  /* Calculate the total length of the concatenated string */
  for (i = 1; i < size - limits - 1; i++) {
    len += strlen(expr[i]);
  }

  /* Allocate memory for the concatenated string (+1 for null terminator) */
  concat = (char *)malloc(sizeof(char) * (len + 1));
  if (concat == NULL) {
    printf("Memory allocation failed in prepare.\n");
    return NULL;
  }

  /* Initialize the string to prevent undefined behavior in strcat */
  concat[0] = '\0';

  /* Concatenate all the strings into `concat` */
  for (i = 1; i < size - limits - 1; i++) {
    strcat(concat, expr[i]);
  }

  printf("Before removing spaces: %s\n", concat);

  /* Remove spaces and replace commas with dots */
  if (remove_spaces(concat) != 0) {
    printf("Error removing spaces from the string.\n");
    free(concat);
    return NULL;
  }

  printf("After removing spaces: %s\n", concat);

  /* Debug output to print ASCII values of the characters */
  len = strlen(concat);
  for (i = 0; i < len; i++) {
    printf("ASCII of '%c': %d\n", concat[i], concat[i]);
  }

  return concat;
}

int is_var(char *p) { return *p == VAR; }

int is_operator(char *p) {
  int i = 0;
  char *ops = OPS;
  for (i = 0; i < (int)strlen(ops); i++) {
    if (*p == ops[i]) {
      return 1;
    }
  }
  return 0;
}

int is_unary_minus(char *p , Token *last) {
	if (*p != '-') return 0;
  if (last->type == TOKEN_VARIABLE || last->type == TOKEN_NUMBER)
    return 0;
  if (last->type == TOKEN_BINARY_OPERATOR && strcmp(last->value, ")") == 0)
    return 0;
  return 1;
}

int is_number(char *str) {
  const char *p = str; /* Pointer to traverse the string */
  int has_digits = 0;  /* Flag to ensure we encounter at least one digit */
  int exponent_digits = 0;                 /* Flag for exponent digits */

  /* Parse the main part of the number */
  while (isdigit(*p)) {
    has_digits = 1; /* Found at least one digit */
    p++;
  }

  /* Check for a decimal point */
  if (*p == '.') {
    p++;
    /* Parse digits after the decimal point */
    while (isdigit(*p)) {
      has_digits = 1; /* Found at least one digit */
      p++;
    }
  }

  /* Check for an exponent (E or e) */
  if ((*p == 'E' || *p == 'e') && has_digits) {
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
  int i = 0, j = 0, len = 0;
  char *functions[] = FUNCTIONS, *func = NULL;
  /* Loop through the array of function names */
  for (i = 0; i < (int)(sizeof(functions) / sizeof(functions[0])); i++) {
		func = functions[i];
		len = strlen(func);
		for (j = 0; j < len; j++)
		{
			if (str[j] != func[j]) break;
		}
		if (j == len) return len;
  }
  return 0; /* Return 0 if no match is found */
}

/* returns count of tokens if positive, error code if negative */
int tokenize(char *expr[], int size, int limits, Token **result) {
  int unknown = 0;
  int free_index = 0;
  char *concat = prepare(expr, size, limits);
  char *p;
  int func_len = 0;
  int num_len = 0;
  int i = 0;

  if (concat == NULL) {
    printf("Error in preparing the input expression.\n");
    return -1;
  }


  p = concat;
  printf("tokenize...%s\n", concat);

  *result = (Token *)malloc(sizeof(Token) *
                            MAX_TOKENS); /* Use a safe upper limit for tokens */
  if (*result == NULL) {
    printf("Memory allocation failed for tokens.\n");
    free(concat);
    return -1;
  }

  printf("size of **result %lu * %lu = %lu\n", sizeof(Token), strlen(concat),
         sizeof(**result));

  while (*p != '\0') {
    printf("character - %c - position - %d\n", *p, free_index);
    func_len = is_function(p);
    num_len = is_number(p);
    printf("remaining: %s\n", p);

    if (is_var(p)) {
      printf("Variable\n");
      (*result)[free_index].type = TOKEN_VARIABLE;
      (*result)[free_index].value[0] = *p;   /* Assign the variable character */
      (*result)[free_index].value[1] = '\0'; /* Null-terminate the value */
      p++;
      free_index++;
    } else if (*p == '(') {
      printf("Parenthesis\n");
      (*result)[free_index].type = TOKEN_LPARENTHESIS;
      (*result)[free_index].value[0] = *p;   /* Assign the variable character */
      (*result)[free_index].value[1] = '\0'; /* Null-terminate the value */
      p++;
      free_index++;
    } else if (*p == ')') {
      printf("Parenthesis\n");
      (*result)[free_index].type = TOKEN_RPARENTHESIS;
      (*result)[free_index].value[0] = *p;   /* Assign the variable character */
      (*result)[free_index].value[1] = '\0'; /* Null-terminate the value */
      p++;
      free_index++;
    } else if (is_operator(p)) {
      printf("Operator\n");
      if (free_index == 0 && *p == '-') {
        (*result)[free_index].type = TOKEN_UNARY_OPERATOR;
      } else if (is_unary_minus(p, &(*result)[free_index - 1])) {
        (*result)[free_index].type = TOKEN_UNARY_OPERATOR;
      } else {
        (*result)[free_index].type = TOKEN_BINARY_OPERATOR;
      }
      (*result)[free_index].value[0] = *p;   /* Assign the operator character */
      (*result)[free_index].value[1] = '\0'; /* Null-terminate the value */
      p++;
      free_index++;
    } else if (func_len) {
      printf("Function len %d\n", func_len);
      (*result)[free_index].type = TOKEN_FUNCTION;
      for (i = 0; i < func_len; i++) {
        (*result)[free_index].value[i] = *p;
        p++;
      }
      (*result)[free_index].value[func_len] =
          '\0'; /* Null-terminate the function name */
      free_index++;
    } else if (num_len) {
      printf("Number len %d\n", num_len);
      (*result)[free_index].type = TOKEN_NUMBER;
      for (i = 0; i < num_len; i++) {
        (*result)[free_index].value[i] = *p;
        p++;
      }
      (*result)[free_index].value[num_len] =
          '\0'; /* Null-terminate the number */
      free_index++;
    } else {
      printf("Unknown token: %c\n", *p);
      unknown++;
      p++;
    }
  }

  free(concat);

  if (unknown) {
    printf("Unknown tokens encountered: %d\n", unknown);
    return -2;
  }

  printf("Total tokens: %d\n", free_index);
  for (i = 0; i < free_index; i++) {
    printf("Token %d: Type = %d, Value = %s\n", i, (*result)[i].type,
           (*result)[i].value);
  }

  return free_index;
}
