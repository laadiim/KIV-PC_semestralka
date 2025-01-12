#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "postfix_eval.h"
#include "postscript.h"
#include "stack.h"
#include "tokens.h"
#include "yard.h"


/* steps for drawing graph */
#define STEPS 2000

#define EXIT_INVALID_ARGS 1
#define EXIT_INVALID_FUNCTION 2
#define EXIT_INVALID_OUTPUT 3
#define EXIT_INVALID_LIMITS 4

/**
 * @brief Check if expression is in mathematically correct format
 *
 * @param expr Expression to evaluate
 *
 * @return 1 if correct, 0 if incorrect
 */
int check_if_correct(Expression *expr) {
  stack *token_stack = NULL; /* token stack for brackets evaluation */
  int i = 0;
  int error = 0;     /* flag, 1 if an error was found */
  Token **token_arr; /* = exp->arr, it just gets annoying to write */

  /* sanity check */
  if (expr == NULL)
    return 0;

  token_arr = expr->arr;

  /* stack creation */
  token_stack = stack_create(expr->len);
  if (token_stack == NULL)
    return 0;

  /* first token cannot be binary operator or right bracket - they need
   * appropriate token before them */
  if (token_arr[0]->type == TOKEN_BINARY_OPERATOR ||
      token_arr[0]->type == TOKEN_RPARENTHESIS)
    error = 1;

  for (i = 0; i < expr->len - 1; i++) {
    /* left bracket found -> push to stack */
    if (token_arr[i]->type == TOKEN_LPARENTHESIS) {
      if (!stack_push(token_stack, token_arr[i]))
        error = 1;
      /* first token inside brackets cannot be binary operator - no left operand
       * - or right bracket - empty brackets are invalid */
      if (token_arr[i + 1]->type == TOKEN_BINARY_OPERATOR ||
          token_arr[i + 1]->type == TOKEN_RPARENTHESIS)
        error = 1;
    }

    /* right bracket -> pop from stack */
    if (token_arr[i]->type == TOKEN_RPARENTHESIS) {
      if (stack_pop(token_stack) == NULL) {
        printf("Closing brackets too soon\n");
        error = 1;
      }
      /* right bracket must be followed by binary operator or another right
       * bracket - multilevel brackets */
      if (token_arr[i + 1]->type != TOKEN_BINARY_OPERATOR &&
          token_arr[i + 1]->type != TOKEN_RPARENTHESIS)
        error = 1;
    }

    /* else check rules */
    /* binary operator cannot be followed by another bianry operator or right
     * barcket */
    if (token_arr[i]->type == TOKEN_BINARY_OPERATOR &&
        (token_arr[i + 1]->type == TOKEN_BINARY_OPERATOR ||
         token_arr[i + 1]->type == TOKEN_RPARENTHESIS))
      error = 1;

    /* unary operator cannot be followed by binary operator, unary operator or
     * right bracket */
    if (token_arr[i]->type == TOKEN_UNARY_OPERATOR &&
        (token_arr[i + 1]->type == TOKEN_BINARY_OPERATOR ||
         token_arr[i + 1]->type == TOKEN_RPARENTHESIS ||
         token_arr[i + 1]->type == TOKEN_UNARY_OPERATOR))
      error = 1;

    /* function call must be followed by left parenthesis */
    if (token_arr[i]->type == TOKEN_FUNCTION &&
        token_arr[i + 1]->type != TOKEN_LPARENTHESIS)
      error = 1;

    /* numbers and variables must be followed by binary operator or right
     * bracket */
    if ((token_arr[i]->type == TOKEN_NUMBER ||
         token_arr[i]->type == TOKEN_VARIABLE) &&
        (token_arr[i + 1]->type != TOKEN_BINARY_OPERATOR &&
         token_arr[i + 1]->type != TOKEN_RPARENTHESIS))
      error = 1;
  }

  /* last is right bracket -> pop */
  if (token_arr[expr->len - 1]->type == TOKEN_RPARENTHESIS) {
    if (stack_pop(token_stack) == NULL) {
      error = 1;
      printf("Closing brackets too soon\n");
    }
  }

  /* last token cannot be binary operator, function call or left bracket */
  if (token_arr[expr->len - 1]->type == TOKEN_BINARY_OPERATOR ||
      token_arr[expr->len - 1]->type == TOKEN_FUNCTION ||
      token_arr[expr->len - 1]->type == TOKEN_LPARENTHESIS)
    error = 1;

  /* if something remains in the stack, unclosed brackets remain */
  if (stack_peek(token_stack) != NULL) {
    printf("Unclosed bracket\n");
    error = 1;
  }
  stack_clear(&token_stack);
  return !error;
}

/**
 * @brief Gets limits values from argument
 *
 * @param arg Argument to parse
 * @param x_min Where to store x_min
 * @param x_max Where to store x_max
 * @param y_min Where to store y_min
 * @param y_max Where to store y_max
 *
 * @return 1 if everything was OK, 0 if args were invalid or argument wasn't
 * formated correctly
 */
int get_limits(char *arg, double *x_min, double *x_max, double *y_min,
               double *y_max) {
  char *endptr = NULL;

  /* sanity check */
  if (arg == NULL || x_min == NULL || x_max == NULL || y_min == NULL ||
      y_max == NULL)
    return 0;

  /* first three numbers must be followed by ':' */
  *x_min = strtod(arg, &endptr);
  if (*endptr != ':')
    return 0;
  arg = endptr + 1;

  *x_max = strtod(arg, &endptr);
  if (*endptr != ':')
    return 0;
  arg = endptr + 1;

  *y_min = strtod(arg, &endptr);
  if (*endptr != ':')
    return 0;
  arg = endptr + 1;

  /* last must be followed by '\0' */
  *y_max = strtod(arg, &endptr);
  if (*endptr != '\0')
    return 0;
  return 1;
}

/**
 * @brief Entry point
 *
 * @param argc Number of commandline arguments
 * @param argv[] Commandline arguments
 *
 * @return 0 if everything was OK, 1 if arguments were invalid, 2 if function
 * wasnt correctly formated, 3 if output file couldn't be created, 4 if limits
 * weren't correctly formated
 */
int main(int argc, char *argv[]) {
  char *out_name = NULL;  /* name of output file */
  Expression *exp = NULL; /* expression */
  int i = 0;

  /* limits */
  double x_min = -10;
  double x_max = 10;
  double y_min = -10;
  double y_max = 10;

  /* steps for drawing graph */
  double step = 0;            /* size of step */
  double x_values[STEPS + 1]; /* computed x values */
  double y_values[STEPS + 1]; /* computed y values */

  int func_error =
      0; /* flag, 1 if error occured during expression evaluation */

  setlocale(LC_NUMERIC, "C"); /* need to standardize locale, later changing
                                 decimal comma to dot */

  /* Sanity check */
  if (argc < 3) {
    printf("Not enough parameters!\n");
    printf("Usage: %s <equation> <output file> [<limits>]\n", argv[0]);
    return EXIT_INVALID_ARGS;
  }

  if (argc == 3) {
    printf("Limits not entered, using -10:10:-10:10.\n");
  }

  /* get limits */
  if (argc == 4) {
    if (!get_limits(argv[3], &x_min, &x_max, &y_min, &y_max)) {
      printf("Invalid limits format!\n");
      printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
      return EXIT_INVALID_LIMITS;
    }

    /* if limit values don't make sense */
    if (x_min >= x_max || y_min >= y_max) {
      printf("Invalid limits values!\n");
      printf("Usage: <x_min>:<x_max>:<y_min>:<y_max>\n");
      return EXIT_INVALID_LIMITS;
    }
  }

  if (argc > 4) {
    printf("Too many parameters!\n");
    printf("Usage: %s <equation> <output file> [<limits>]\n", argv[0]);
    return EXIT_INVALID_ARGS;
  }

  /* get ouput file name */
  out_name = argv[2];

  /* tokenize */
  exp = tokenize(argv[1]);

  /* negative length signifies error, length of 0 is useless for further
   * computation */
  if (exp->len <= 0) {
    printf("Function was not in the correct format!\n");
    free_expression(&exp);
    return EXIT_INVALID_FUNCTION;
  }

  /* check correctness */
  if (!check_if_correct(exp)) {
    printf("Function was not in mathematically correct format!\n");
    free_expression(&exp);
    return EXIT_INVALID_FUNCTION;
  }

  /* transform to postfix, new length is returned */
  /* negative length signifies error, length of 0 is useless for further
   * computation */
  if (shunting_yard(exp) <= 0) {
    printf("Function was not in the correct format!\n");
    free_expression(&exp);
    return EXIT_INVALID_FUNCTION;
  }

  /* evaluate for required numbers */
  step = (x_max - x_min) / (STEPS);

  for (i = 0; i <= STEPS; i++) {
    x_values[i] = x_min + i * step;

    /* if 0 was returned, something went wrong during calculation */
    if (!postfix_eval(exp, x_values[i], &(y_values[i]))) {
      printf("Function could not be evaluated!\n");
      func_error = 1;
      break;
    }
  }

  if (func_error) {
    printf("Function was not in the correct format!\n");
    free_expression(&exp);
    return EXIT_INVALID_FUNCTION;
  }

  /* create output file */
  /* if return value was 0, output couldn't be created */
  if (!create_postscript(x_values, y_values, STEPS + 1, x_min, x_max, y_min,
                         y_max, out_name)) {
    printf("Couldn't create output file!\nCheck your system requirements for "
           "naming files.\n");
    free_expression(&exp);
    return EXIT_INVALID_OUTPUT;
  }

  free_expression(&exp);
  return EXIT_SUCCESS;
}
