typedef enum {
    NUMVAR,
    CHARVAR,
    LAMBDA,
    OPEN,
    CLOSE
} TokenType;

typedef struct Token {
    TokenType type;
    union {
        int index;
        char name;
    };
} Token;

typedef struct TokenArray {
    int length;
    Token *tokens;
} TokenArray;

TokenArray *tokenize(char *);

void free_token_array(TokenArray *);

int is_token_variable(Token *);

int has_outmost_parentheses(TokenArray *, int, int);

int get_rightmost_group_size(TokenArray *, int, int);