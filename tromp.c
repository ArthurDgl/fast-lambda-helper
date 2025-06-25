#include "tromp.h"
#include <stdlib.h>
#include <stdio.h>

Binder *make_binder(int depth, int offset, Binder *next) {
    Binder *new = (Binder *)malloc(sizeof(Binder));
    new->depth = depth;
    new->offset = offset;
    new->next = next;
    return new;
}

void free_binder(Binder *binder) {
    if (binder->next) free_binder(binder->next);
    free(binder);
}

BinderList *make_binder_list() {
    BinderList *new = (BinderList *)malloc(sizeof(BinderList));
    new->first = NULL;
    return new;
}

void push_to_list(BinderList *list, int value, int offset) {
    list->first = make_binder(value, offset, list->first);
}

void add_list_to_top(BinderList *list1, BinderList *list2) {
    Binder *last = list2->first;
    while (last->next != NULL) last = last->next;

    last->next = list1->first;
    list1->first = list2->first;
}

void free_binder_list(BinderList *list) {
    if (list->first) free_binder(list->first);
    free(list);
}

Grid *make_grid(int width, int height) {
    Grid *new = (Grid *)malloc(sizeof(Grid));
    new->width = width;
    new->height = height;
    new->values = (int *)malloc(width * height * sizeof(int));
    return new;
}

void print_grid(Grid *grid) {
    if (!grid) return;
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            int val = grid->values[x * grid->height + y];
            if (val) printf("# ");
            else printf("  ");
        }
        printf("\n");
    }
}

int is_in_bounds(Grid *grid, int x, int y) {
    if (!grid) return 0;
    return x >= 0 && x < grid->width && y >= 0 && y < grid->height;
}

void set_grid(Grid *grid, int x, int y, int value) {
    if (!is_in_bounds(grid, x, y)) return;
    grid->values[x * grid->height + y] = value;
}

void fill_grid(Grid *grid, int value) {
    if (!grid) return;
    for (int x = 0; x < grid->width; x++) {
        for (int y = 0; y < grid->height; y++) {
            grid->values[x * grid->height + y] = value;
        }
    }
}

void clone_grid_in_grid(Grid *parent, Grid *child, int ox, int oy) {
    if (!parent || !child) return;
    for (int i = 0; i < child->width; i++) {
        for (int j = 0; j < child->height; j++) {
            parent->values[(ox + i) * parent->height + oy + j] = child->values[i * child->height + j];
        }
    }
}

void fill_rect_in_grid(Grid *grid, int x1, int y1, int x2, int y2, int value) {
    if (x1 > x2) {
        int t = x2;
        x2 = x1;
        x1 = t;
    }
    if (y1 > y2) {
        int t = y2;
        y2 = y1;
        y1 = t;
    }
    Grid *temp = make_grid(x2 - x1 + 1, y2 - y1 + 1);
    fill_grid(temp, value);
    clone_grid_in_grid(grid, temp, x1, y1);
    free_grid(temp);
}

void free_grid(Grid *grid) {
    if (!grid) return;
    free(grid->values);
    free(grid);
}

Grid *create_tromp_diagram(Expression *expression, BinderList *binders) {
    if (!expression) return make_grid(0, 0);

    if (expression->type == VARIABLE) {
        push_to_list(binders, expression->index, 2);
        Grid *grid = make_grid(1, 2);
        fill_grid(grid, 1);
        return grid;
    }

    if (expression->type == ABSTRACTION) {
        Grid *child_grid = create_tromp_diagram(expression->body, binders);

        int offset_left = 1;
        int offset_right = 1;
        if (has_left_side_abstraction(expression->body)) offset_left = 0;
        if (has_right_side_abstraction(expression->body)) offset_right = 0;

        Grid *this_grid = make_grid(child_grid->width + offset_left + offset_right, child_grid->height + 2);
        fill_grid(this_grid, 0);
        
        clone_grid_in_grid(this_grid, child_grid, offset_left, 2);

        fill_rect_in_grid(this_grid, 0, 1, this_grid->width - 1, 1, 1);

        int x = this_grid->width - 2;
        for (Binder *bind = binders->first; bind != NULL; bind = bind->next) {
            if (bind->depth > 0) {
                set_grid(this_grid, x, 0, 1);
                bind->depth--;
            }
            if (offset_left && bind->next == NULL) {
                bind->offset++;
            }
            x -= bind->offset;
        }
        free_grid(child_grid);

        return this_grid;
    }

    if (expression->type == APPLICATION) {
        Grid *func_grid = create_tromp_diagram(expression->app.function, binders);
        BinderList *arg_binders = make_binder_list();
        Grid *arg_grid = create_tromp_diagram(expression->app.argument, arg_binders);
        add_list_to_top(binders, arg_binders);
        free(arg_binders);
        arg_binders = NULL;

        int height = func_grid->height + 1;
        if (arg_grid->height > height) height = arg_grid->height;
        height++;

        Grid *this_grid = make_grid(func_grid->width + 1 + arg_grid->width, height);
        fill_grid(this_grid, 0);

        clone_grid_in_grid(this_grid, func_grid, 0, 0);
        int arg_grid_origin = func_grid->width + 1;
        clone_grid_in_grid(this_grid, arg_grid, arg_grid_origin, 0);

        int root_offset_1 = 0;
        if (has_left_side_abstraction(expression->app.function)) root_offset_1++;
        int root_offset_2 = 0;
        if (has_left_side_abstraction(expression->app.argument)) root_offset_2++;

        fill_rect_in_grid(this_grid, root_offset_1, func_grid->height, root_offset_1, height - 1, 1);

        int app_bar_y = height - 2;
        fill_rect_in_grid(this_grid, root_offset_1, app_bar_y, arg_grid_origin + root_offset_2, app_bar_y, 1);

        if (app_bar_y > arg_grid->height) {
            fill_rect_in_grid(this_grid, arg_grid_origin + root_offset_2, arg_grid->height, arg_grid_origin + root_offset_2, app_bar_y, 1);
        }

        free_grid(func_grid);
        free_grid(arg_grid);

        return this_grid;
    }
}

void render_tromp_diagram(Expression *expression) {
    Grid *result = create_tromp_diagram(expression, make_binder_list());
    printf("Tromp diagram for expression < ");
    print_expression_r(expression, 1, 0);
    printf(" > is :\n");
    print_grid(result);
    free_grid(result);
}