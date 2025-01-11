#ifndef tokens
#define tokens
 
/* Enumeration for different token types */
typedef enum {
    TOKEN_NUMBER,    /* Numeric constants (e.g., 3.14, 42) */
    TOKEN_VARIABLE,  /* Variable (e.g., 'x') */
    TOKEN_BINARY_OPERATOR,  /* Operators (e.g., +, -, *, /, ^) */
		TOKEN_LPARENTHESIS,
		TOKEN_RPARENTHESIS,
    TOKEN_FUNCTION,  /* Functions (e.g., sin, cos, exp) */
		TOKEN_UNARY_OPERATOR, /* Unary '-' */
    TOKEN_UNKNOWN    /* Any unrecognized token */
} TokenType;

/* Structure to represent a token */
typedef struct {
    TokenType type;    /* Type of the token */
    char value[32];    /* String value of the token (e.g., "3.14", "sin") */
} Token;

typedef struct {
		Token **arr;
		int len;
} Expression;

/* Saves tokenized expression at result
 * returns
 * number of tokens if succesfull, -1 otherwise */
Expression *tokenize(char *expr);

/* removes spaces and changes commas to periods 
 * changes string in place
 * returns
 * 0 if succesful, 1 otherwise*/
int remove_spaces(char *str);

/* prepares expression for processing 
 * returns
 * prepared string if successful, NULL othervise */
char *prepare(char **expr, int size, int limits);

void free_expression(Expression **expr);

#endif
