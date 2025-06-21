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

void print_expression(Expression *expression) {
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
            print_expression_r(expression->app.argument, 0, 0);
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