#include "lambdas.h"
#include <stdio.h>


int main() {
    Expression *expression = parse_expression("(L.f0(L.L.0)(L.L.1))(L.L.1)");
    print_expression(expression);
    print_tree(expression, 0);

    return 0;
}