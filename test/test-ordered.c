#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifndef RBTREE_TYPE
#define RBTREE_TYPE void
#endif

#include <rbtree.h>
#include <orderstat.h>

int int_compare(RBTREE_TYPE *a, RBTREE_TYPE *b)
{
    int x = *((int *)a);
    int y = *((int *)b);

    return x-y;
}

void ordered_print(rb_node *a) {
    printf("%d / %d",
	    *(int *)(a->key),
	    a->size);
}

rb_node *mknode(int i)
{
    int *key = malloc(sizeof(*key));
    *key = i;
    rb_node *z = rb_new_node(key);
    return z;
}

void test_tree()
{
    rb_tree *tree = rb_new_tree(int_compare, ordered_print);
    rb_node *x;
    int i;
    /* copy tree from 14.1 in CLRS */
    int nodes[] = {26, 17, 41, 14, 21, 30, 47, 10, 16, 19, 21, 28, 38, 7, 12,
	14, 20, 35, 39, 3, 0};

    for (i=0; nodes[i] != 0; i++)
	rb_insert(tree, mknode(nodes[i]));
    rb_print_tree(tree, RB_TREE_DOT);

    x = ordered_select(tree, 17);

    assert (ordered_rank(tree, x) == 17);

    /* peek inside the data structure to ensure it all updated */
    x = ordered_select(tree, 1);
    x = rb_delete(tree, x);
    assert(tree->root->size == 19);
}

int main(int argc, char **argv)
{
    test_tree();
    return 0;
}
