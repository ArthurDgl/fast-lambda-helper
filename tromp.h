#include "lambdas.h"

typedef struct Binder {
    int depth;
    int offset;
    struct Binder *next;
} Binder;

typedef struct BinderList {
    struct Binder *first;
} BinderList;

typedef struct Grid {
    int width;
    int height;
    int *values;
} Grid;

Binder *make_binder(int, int, Binder *);

void free_binder(Binder *);

BinderList *make_binder_list();

void push_to_list(BinderList *, int, int);

void add_list_to_top(BinderList *, BinderList *);

void free_binder_list(BinderList *);

Grid *make_grid(int, int);

int is_in_bounds(Grid *, int, int);

void set_grid(Grid *, int, int, int);

void fill_grid(Grid *, int);

void clone_grid_in_grid(Grid *, Grid *, int, int);

void fill_rect_in_grid(Grid *, int, int, int, int, int);

void free_grid(Grid *);

Grid *create_tromp_diagram(Expression *, BinderList *);

void render_tromp_diagram(Expression *);