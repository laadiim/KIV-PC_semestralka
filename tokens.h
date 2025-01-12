#ifndef tokens
#define tokens
 
/* enumeration for different token types */
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

/* structure to represent a token */
typedef struct {
    TokenType type;    /* Type of the token */
    char value[32];    /* String value of the token (e.g., "3.14", "sin") */
} Token;

/* structure to represent an expression */
typedef struct {
		Token **arr; /* array of token */
		int len; /* number of tokens in the expression */
} Expression;

/**
 * @brief Breaks up the string into tokens
 *
 * @param expr String of an expression
 *
 * @return Pointer to an Expression struct with tokenized expression, NULL if an error occured
 */
Expression *tokenize(char *expr);

/**
 * @brief Removes spaces from the string and repaces commas for dots
 *
 * @param str String to process
 *
 * @return 1 if everything was OK, 0 if an error occured
 */
int remove_spaces_replace_commas(char *str);


/**
 * @brief Frees memory allocated for an Expression
 *
 * @param expr Expression
 */
void free_expression(Expression **expr);

#endif
