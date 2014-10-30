#include "rbtree.h"

rb_node * ordered_select_do(rb_tree *T, rb_node *x, int i)
{
    int r;

    r = x->left->size + 1;
    if (i == r)
	return x;
    else if (i<r)
	return(ordered_select_do(T, x->left, i));
    else
	return (ordered_select_do(T, x->right, i-r));
}

rb_node * ordered_select(rb_tree *T, int rank)
{
    return (ordered_select_do(T, T->root, rank));
}

int ordered_rank(rb_tree *T, rb_node *x)
{
    int r = x->left->size + 1;
    rb_node *y = x;

    while (y != T->root) {
	if (y == y->p->right)
	    r = r + y->p->left->size + 1;
	y = y->p;
    }
    return r;
}
