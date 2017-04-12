#ifndef TREE_H
#define TREE_H

#ifndef TREE_TYPE
#define TREE_TYPE void
#endif

enum tree_output_flags {
    TREE_DEFAULT = 0x1,
    TREE_DOT     = 0x2,
};

typedef struct node {
    struct node *left;
    struct node *right;
    struct node *p;
    TREE_TYPE *key;
    void * value;
} node;

typedef struct {
    node *root;
    node *nil;
    int (*compare)(TREE_TYPE *a, TREE_TYPE *b);
    void (*print)();
} tree;


node * tree_node(void *key);
tree *tree_alloc(int (*compare)(TREE_TYPE *a, TREE_TYPE *b),
	void (*print)());

void tree_walk_inorder(tree *T, node *n);

node *tree_search(tree *T, node *x, TREE_TYPE *k);
node *tree_min(tree *T, node *x);
node *tree_max(tree *T, node *x);
node *tree_successor(tree *T, node *x);
void tree_insert(tree *T, node *z, int unique);
void tree_delete(tree *T, node *z);
int tree_height(tree *T);
void tree_print(tree *t, int format_flags);

#endif
