#include "tokens.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKENS 256
#define VAR 'x'
#define OPS "+-*/^"
#define FUNCTIONS {"asin", "acos", "atan", "sinh", "cosh", "tanh", "sin",  "cos",  "tan",  "abs", "exp",  "log", "ln"}

int remove_spaces_replace_commas(char *str) {
  int i = 0, j = 0;

  /* sanity check */
  if (str == NULL)
    return 0;

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
  return j;
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

void free_expression(Expression **expr)
{
	int i = 0;
	if (expr == NULL || *expr == NULL) return;
	for (i = 0; i < (*expr)->len; i++) {
		free((*expr)->arr[i]);
		(*expr)->arr[i] = NULL;
	}
	free((*expr)->arr);
	(*expr)->arr = NULL;
	free(*expr);
	expr = NULL;
}

/* returns count of tokens if positive, error code if negative */
Expression *tokenize(char *expr) {
  int unknown = 0;
  int free_index = 0;
  char *p;
  int func_len = 0;
  int num_len = 0;
  int i = 0;
	int len = 0;
	Token **array = NULL;
	Token *curr = NULL;
	Expression *result = NULL;

	len = remove_spaces_replace_commas(expr);
	if (!len)
	{
		printf("Error while removing spaces!\n");
		return NULL;
	}

  p = expr;
  printf("tokenize...%s\n", expr);

  array = (Token **)malloc(sizeof(Token *) * strlen(expr));
  if (array == NULL) {
    printf("Memory allocation failed for tokens.\n");
    return NULL;
  }

  while (*p != '\0') {
    func_len = is_function(p);
    num_len = is_number(p);

		curr = (Token *)(malloc(sizeof(Token)));
		if (curr == NULL)
		{
			free(array);
			printf("Error at char %c\n", *p);
			return NULL;
		}

    if (is_var(p)) {
      printf("Variable\n");
      curr->type = TOKEN_VARIABLE;
      curr->value[0] = *p;   /* Assign the variable character */
      curr->value[1] = '\0'; /* Null-terminate the value */
      p++;

    } else if (*p == '(') {
      printf("Parenthesis\n");
      curr->type = TOKEN_LPARENTHESIS;
      curr->value[0] = *p;   /* Assign the variable character */
      curr->value[1] = '\0'; /* Null-terminate the value */
      p++;

    } else if (*p == ')') {
      printf("Parenthesis\n");
      curr->type = TOKEN_RPARENTHESIS;
      curr->value[0] = *p;   /* Assign the variable character */
      curr->value[1] = '\0'; /* Null-terminate the value */
      p++;

    } else if (is_operator(p)) {
      printf("Operator\n");
      if (free_index == 0 && *p == '-') {
        curr->type = TOKEN_UNARY_OPERATOR;

    } else if (is_unary_minus(p, array[free_index - 1])) {
        curr->type = TOKEN_UNARY_OPERATOR;
      
			} else {
        curr->type = TOKEN_BINARY_OPERATOR;
      }
      
			curr->value[0] = *p;   /* Assign the operator character */
      curr->value[1] = '\0'; /* Null-terminate the value */
      p++;
    
		} else if (func_len) {
      printf("Function len %d\n", func_len);
      curr->type = TOKEN_FUNCTION;
      for (i = 0; i < func_len; i++) {
        curr->value[i] = *p;
        p++;
      }
      curr->value[func_len] =
          '\0'; /* Null-terminate the function name */
		
		} else if (num_len) {
      printf("Number len %d\n", num_len);
      curr->type = TOKEN_NUMBER;
      
			for (i = 0; i < num_len; i++) {
        curr->value[i] = *p;
        p++;
      }
    
			curr->value[num_len] =
          '\0'; /* Null-terminate the number */
    
		} else {
      printf("Unknown token: %c\n", *p);
			free(curr);
			free(array);
			return NULL;
    }
		printf("%d, %s\n", curr->type, curr->value);
		array[free_index] = curr;
		printf("%d, %s\n", array[free_index]->type, array[free_index]->value);
		free_index++;
  }

	result = (Expression *)(malloc(sizeof(Expression)));
	if (result == NULL)
	{
		printf("Error while allocating memory.\n");
		for (i = 0; i < free_index; i++)
		{
			free(array[i]);
		}
		free(array);
		return NULL;
	}


	result->arr = array;
	result->len = free_index;

  return result;
}
