#include "tromp.h"
#include <stdio.h>


int main() {
    Expression *expression = parse_expression("(L.L.1 0 (L.L.0))(L.L.1)(L.L.1)");
    render_tromp_diagram(expression);

    expression = search_beta_normal_form(expression, 1);
    render_tromp_diagram(expression);

    return 0;
}