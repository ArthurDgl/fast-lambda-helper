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

Expression *clone_expression(Expression *);

void free_expression(Expression *);

void print_all(Expression *);

void print_expression(Expression*);

void print_expression_r(Expression*, int, int);

void print_tree(Expression *, int);

void print_depth(int);

Expression *parse_tokens(TokenArray *, int, int);

Expression *parse_expression(char *);

int is_expression_valid(Expression *);

int has_left_side_abstraction(Expression *);

int has_right_side_abstraction(Expression *);

void substitute_abstraction(Expression *, int, Expression *);

void substitute_application(Expression *, int, Expression *);

int is_redex(Expression *);

Expression *beta_reduce(Expression *);

Expression *reduce_normal_order(Expression *);

int reduce_normal_order_r(Expression *);

Expression *search_beta_normal_form(Expression *, int);