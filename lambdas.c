#include "lambdas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Expression *make_variable(int index) {
    Expression *expression = malloc(sizeof(Expression));

    expression->type = VARIABLE;
    expression->index = index;

    return expression;
}

Expression *make_abstraction(Expression *body) {
    Expression *expression = malloc(sizeof(Expression));

    expression->type = ABSTRACTION;
    expression->body = body;

    return expression;
}

Expression *make_application(Expression *function, Expression *argument) {
    Expression *expression = malloc(sizeof(Expression));

    expression->type = APPLICATION;
    expression->app.function = function;
    expression->app.argument = argument;

    return expression;
}

Expression *clone_expression(Expression *expression) {
    if (!expression) return NULL;

    switch (expression->type) {
        case VARIABLE:
            return make_variable(expression->index);
        case ABSTRACTION:
            return make_abstraction(clone_expression(expression->body));
        case APPLICATION:
            return make_application(
                clone_expression(expression->app.function),
                clone_expression(expression->app.argument)
            );
    }
}

void free_expression(Expression *expression) {
    if (!expression) return;
    switch (expression->type) {
        case VARIABLE:
            break;
        case ABSTRACTION:
            free_expression(expression->body);
            break;
        case APPLICATION:
            free_expression(expression->app.function);
            free_expression(expression->app.argument);
            break;
    }
    free(expression);
}

void print_all(Expression *expression) {
    printf("Lambda expression < ");
    print_expression_r(expression, 1, 0);
    printf(" > has tree :\n");
    print_tree(expression, 0);
}

void print_expression(Expression *expression) {
    if (!expression) return;

    print_expression_r(expression, 1, 0);
    printf("\n");
}

void print_expression_r(Expression *expression, int top, int lambda) {
    if (!expression) return;

    switch (expression->type) {
        case VARIABLE:
            if (expression->index >= 0)printf("%d", expression->index);
            else printf("%c", (char)-expression->index);
            break;
        case ABSTRACTION:
            if (!top && !lambda) printf("(");
            printf("L. ");
            print_expression_r(expression->body, 0, 1);
            if (!top && !lambda) printf(")");
            break;
        case APPLICATION:
            print_expression_r(expression->app.function, 0, 0);
            printf(" ");
            if (expression->app.argument->type == APPLICATION) printf("(");
            print_expression_r(expression->app.argument, 0, 0);
            if (expression->app.argument->type == APPLICATION) printf(")");
            break;
    }
}

void print_tree(Expression *expression, int depth) {
    if (!expression) return;

    switch (expression->type) {
        case VARIABLE:
            print_depth(depth);
            if (expression->index >= 0)printf("%d\n", expression->index);
            else printf("%c\n", (char)-expression->index);
            break;
        case ABSTRACTION:
            print_depth(depth);
            printf("L.\n");
            print_tree(expression->body, depth + 1);
            break;
        case APPLICATION:
            print_depth(depth);
            printf("app\n");
            print_tree(expression->app.function, depth + 1);
            print_depth(depth);
            printf("to\n");
            print_tree(expression->app.argument, depth + 1);
    }
}

void print_depth(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("| ");
    }
}

Expression *parse_tokens(TokenArray *array, int start, int end) {
    if (has_outmost_parentheses(array, start, end)) {
        start++;
        end--;
    }

    if (start >= end) return NULL;

    Expression *new;

    if (end - start == 1) {
        int index;
        if (array->tokens[start].type == NUMVAR) index = array->tokens[start].index;
        else if (array->tokens[start].type == CHARVAR) index = -(int) array->tokens[start].name;
        else index = -1000;
        new = make_variable(index);
    } else if (array->tokens[start].type == LAMBDA) {
        new = make_abstraction(parse_tokens(array, start + 1, end));
    }
    else {
        int first_end = end - get_rightmost_group_size(array, start, end);
        new = make_application(parse_tokens(array, start, first_end), parse_tokens(array, first_end, end));
    }

    return new;
}

Expression *parse_expression(char *str) {
    TokenArray *array = tokenize(str);
    Expression *expression = parse_tokens(array, 0, array->length);
    free_token_array(array);
    return expression;
}

int is_expression_valid(Expression *expression) {
    if (!expression) return 0;

    switch (expression->type) {
        case VARIABLE:
            return expression->index != -1000;
        case ABSTRACTION:
            return is_expression_valid(expression->body);
        case APPLICATION:
            return is_expression_valid(expression->app.function) && is_expression_valid(expression->app.argument);
    }
}

int has_left_side_abstraction(Expression *expression) {
    if (!expression) return 0;
    switch (expression->type) {
        case VARIABLE:
            return 0;
        case ABSTRACTION:
            return 1;
        case APPLICATION:
            return has_left_side_abstraction(expression->app.function);
    }
}

int has_right_side_abstraction(Expression *expression) {
    if (!expression) return 0;
    switch (expression->type) {
        case VARIABLE:
            return 0;
        case ABSTRACTION:
            return 1;
        case APPLICATION:
            return has_right_side_abstraction(expression->app.argument);
    }
}

void substitute_abstraction(Expression *expression, int depth, Expression *replacement) {
    if (!expression) return;

    if (expression->type != ABSTRACTION) return;

    switch (expression->body->type) {
        case VARIABLE:
            if (expression->body->index == depth) {
                free_expression(expression->body);
                expression->body = clone_expression(replacement);
            }
            break;
        case ABSTRACTION:
            substitute_abstraction(expression->body, depth + 1, replacement);
            break;
        case APPLICATION:
            substitute_application(expression->body, depth, replacement);
            break;
    }
}

void substitute_application(Expression *expression, int depth, Expression *replacement) {
    if (!expression) return;

    if (expression->type != APPLICATION) return;

    switch (expression->app.function->type) {
        case VARIABLE:
            if (expression->app.function->index == depth) {
                free_expression(expression->app.function);
                expression->app.function = clone_expression(replacement);
            }
            break;
        case ABSTRACTION:
            substitute_abstraction(expression->app.function, depth + 1, replacement);
            break;
        case APPLICATION:
            substitute_application(expression->app.function, depth, replacement);
            break;
    }

    switch (expression->app.argument->type) {
        case VARIABLE:
            if (expression->app.argument->index == depth) {
                free_expression(expression->app.argument);
                expression->app.argument = clone_expression(replacement);
            }
            break;
        case ABSTRACTION:
            substitute_abstraction(expression->app.argument, depth + 1, replacement);
            break;
        case APPLICATION:
            substitute_application(expression->app.argument, depth, replacement);
            break;
    }
}

int is_redex(Expression *expression) {
    if (!expression) return 0;
    if (expression->type != APPLICATION) return 0;

    return expression->app.function->type == ABSTRACTION;
}

Expression *beta_reduce(Expression *expression) {
    if (!expression) return NULL;
    if (expression->type != APPLICATION) return NULL;
    if (!is_redex(expression)) return NULL;

    substitute_abstraction(expression->app.function, 0, expression->app.argument);
    Expression *result = expression->app.function->body;

    expression->app.function->body = NULL;
    free_expression(expression);

    return result;
}

Expression *reduce_normal_order(Expression *expression) {
    if (!expression) return 0;

    if (is_redex(expression)) return beta_reduce(expression);

    if (!reduce_normal_order_r(expression)) return NULL;

    return expression;
}

int reduce_normal_order_r(Expression *expression) {
    if (!expression) return 0;

    switch (expression->type) {
        case VARIABLE:
            return 0;
        case ABSTRACTION:
            if (is_redex(expression->body)) {
                expression->body = beta_reduce(expression->body);
                return 1;
            }
            return reduce_normal_order_r(expression->body);
        case APPLICATION:
            if (is_redex(expression->app.function)) {
                expression->app.function = beta_reduce(expression->app.function);
                return 1;
            }
            if (reduce_normal_order_r(expression->app.function)) {
                return 1;
            }
            if (is_redex(expression->app.argument)) {
                expression->app.argument = beta_reduce(expression->app.argument);
                return 1;
            }
            return reduce_normal_order_r(expression->app.argument);
    }
}

Expression *search_beta_normal_form(Expression *expression, int show) {
    int steps = 0;
    Expression *result = expression;
    if (show) printf("Searching for beta normal form...\n");
    while (result) {
        expression = result;
        if (show) print_expression(expression);
        result = reduce_normal_order(result);
        steps++;
        if (steps >= 10000) {
            if (show) printf("STOPPED AFTER %d STEPS\n", steps);
            return NULL;
        }
    }
    if (show) printf("Found beta normal form %d in steps\n", steps);
    return expression;
}