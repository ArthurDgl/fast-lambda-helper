#include "tokens.h"

typedef enum {
    VARIABLE,
    ABSTRACTION,
    APPLICATION
} LambdaType;

typedef struct Expression {
    LambdaType type;
    union {
        int index;

        struct Expression *body;
        
        struct {
            struct Expression *function;
            struct Expression *argument;
        } app;
    };
    
} Expression;

Expression *make_variable(int);

Expression *make_abstraction(Expression *);

Expression *make_application(Expression *, Expression *);

void free_expression(Expression *);

void print_expression(Expression*);

void print_expression_r(Expression*, int, int);

void print_tree(Expression *, int);

void print_depth(int);

Expression *parse_tokens(TokenArray *, int, int);

Expression *parse_expression(char *);

int is_expression_valid(Expression *);