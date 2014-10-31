#ifndef _RBTREE_H
#define _RBTREE_H

/* for a bit of type safety you can define RBTREE_TYPE before including
 * rbtree.h .  If you do not, the usual C style "void *" will be used
 * everywhere */
#ifndef RBTREE_TYPE
#define RBTREE_TYPE void
#endif

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
    RBTREE_TYPE *key;
    void *value;
    struct rb_node *left;
    struct rb_node *right;
    struct rb_node *p;
   /* additional fields for augmented data strucutres */
    /* ordered-statistics: */
    int size;
    /* interval tree */
    RBTREE_TYPE *low;
    RBTREE_TYPE *high;
    RBTREE_TYPE *max;
} rb_node;

typedef struct {
    rb_node *root;
    rb_node *nil; /* sentinal as used in CLRS */
    int (*compare)(RBTREE_TYPE *a, RBTREE_TYPE *b);
    void (*print)(rb_node *a);
    /* useful for augmented data structures such as interval tree */
    void (*copy) (RBTREE_TYPE *dest, RBTREE_TYPE *src);
}rb_tree;

/* maybe the compare function should take an argument, like qsort_r ? */
rb_tree * rb_new_tree(int (*compare)(RBTREE_TYPE *a, RBTREE_TYPE *b),
	void (*print)(rb_node *a));
rb_node * rb_new_node(RBTREE_TYPE *key);
void rb_insert(rb_tree *t, rb_node *z);
rb_node *rb_search(rb_tree *t, RBTREE_TYPE *k);
rb_node *rb_delete(rb_tree *T, rb_node *z);
void rb_print_tree(rb_tree *t, int format_flags);

#endif
