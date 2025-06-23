#include "lambdas.h"
#include <stdio.h>


int main() {
    Expression *expression = parse_expression("(L.L.L.L.2 3 1 0)(L.L.1(1 0))(L.L.1(1(1 0)))");
    print_all(expression);

    expression = search_beta_normal_form(expression, 1);
    print_all(expression);

    return 0;
}