#ifndef tokens
#define tokens

/* Enumeration for different token types */
typedef enum {
    TOKEN_NUMBER,    /* Numeric constants (e.g., 3.14, 42) */
    TOKEN_VARIABLE,  /* Variable (e.g., 'x') */
    TOKEN_BINARY_OPERATOR,  /* Operators (e.g., +, -, *, /, ^) */
		TOKEN_PARENTHESIS,
    TOKEN_FUNCTION,  /* Functions (e.g., sin, cos, exp) */
		TOKEN_UNARY_OPERATOR, /* Unary '-' */
    TOKEN_UNKNOWN    /* Any unrecognized token */
} TokenType;

/* Structure to represent a token */
typedef struct {
    TokenType type;    /* Type of the token */
    char value[32];    /* String value of the token (e.g., "3.14", "sin") */
} Token;

/* Saves tokenized expression at result */
int tokenize(char **expr, int size, int limits, Token **result);

#endif
