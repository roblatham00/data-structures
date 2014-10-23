enum reb_black_color {
    RED,
    BLACK,
};

enum rb_tree_output_flags {
    RB_TREE_DEFAULT = 0x1,
    RB_TREE_DOT     = 0x2,
};

typedef struct rb_node {
    int color;
    void *key;
    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *p;
} rb_node;

typedef struct {
    rb_node *root;
    rb_node *nil; /* sentinal as used in CLRS */
    int (*compare)(rb_node *a, rb_node *b);
    void (*print)(rb_node *a);
}rb_tree;

rb_tree * rb_new_tree(int (*compare)(rb_node *a, rb_node *b), void (*print)(rb_node *a));
rb_node * rb_new_node(void *key);
void rb_insert(rb_tree *t, rb_node *z);
rb_node *rb_delete(rb_tree *T, rb_node *z);
void rb_print_tree(rb_tree *t, int format_flags);
