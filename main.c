#include "lambdas.h"
#include <stdio.h>


int main() {
    Expression *expression = parse_expression("(L.0(L.L.0)(L.L.1))(L.L.1)");
    print_all(expression);

    expression = beta_reduce(expression);
    print_all(expression);

    expression->app.function = beta_reduce(expression->app.function);
    print_all(expression);

    expression = beta_reduce(expression);
    print_all(expression);

    return 0;
}