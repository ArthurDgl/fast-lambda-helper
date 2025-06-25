#include "tokens.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

TokenArray *tokenize(char *str) {
    int len = strlen(str);
    Token *container = (Token *)malloc(len * sizeof(Token));
    int tokens = 0;

    int currentLength = 0; // number of unused characters (numerals)
    int abs = 0;

    for (int i = 0; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            currentLength++;
            continue;
        }
        else if (currentLength > 0) {
            container[tokens].type = NUMVAR;
            sscanf(str + i - currentLength, "%d", &container[tokens].index);
            tokens++;
            currentLength = 0;
        }

        if (str[i] == 'L') {
            abs = 1;
        }
        else if (str[i] == '.') {
            if (abs == 0) continue;
            abs = 0;
            container[tokens].type = LAMBDA;
            tokens++;
        }
        else if (str[i] >= 'a' && str[i] <= 'z' || str[i] >= 'A' && str[i] <= 'Z') {
            container[tokens].type = CHARVAR;
            container[tokens].name = str[i];
            tokens++;
        }
        else if (str[i] == '(') {
            container[tokens].type = OPEN;
            tokens++;
        }
        else if (str[i] == ')') {
            container[tokens].type = CLOSE;
            tokens++;
        }
    }

    if (currentLength > 0) {
        container[tokens].type = NUMVAR;
        sscanf(str + len - currentLength, "%d", &container[tokens].index);
        tokens++;
        currentLength = 0;
    }

    TokenArray *result = (TokenArray *)malloc(sizeof(TokenArray));
    result->length = tokens;
    result->tokens = (Token *)malloc(tokens * sizeof(Token));
    
    for (int i = 0; i < tokens; i++) {
        result->tokens[i] = container[i];
    }

    free(container);
    container = NULL;

    return result;
}

void free_token_array(TokenArray *array) {
    if (!array) return;
    free(array->tokens);
    free(array);
}

int is_token_variable(Token *token) {
    switch (token->type) {
        case NUMVAR:
            return 1;
        case CHARVAR:
            return 1;
        default:
            return 0;
    }
}

int has_outmost_parentheses(TokenArray *array, int start, int end) {
    if (start >= end) return 0;
    int depth = 0;
    for (int i = start; i < end - 1; i++) {
        if (array->tokens[i].type == OPEN) depth++;
        else if (array->tokens[i].type == CLOSE) depth--;
        
        if (depth == 0) return 0;
    }
    if (depth == 1 && array->tokens[end - 1].type == CLOSE) return 1;
    return 0;
}

int get_rightmost_group_size(TokenArray *array, int start, int end) {
    if (start >= end) return 0;
    int size = 0;
    int depth = 0;
    for (int i = end - 1; i >= start; i--) {
        if (array->tokens[i].type == OPEN) depth--;
        else if (array->tokens[i].type == CLOSE) depth++;
        size++;
        if (depth == 0) return size;
    }
    return size;
}