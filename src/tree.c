#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

node * tree_node(void *key)
{
    node *n = malloc(sizeof(node));
    n->key = key;
    n->value = NULL;
    n->left = NULL;
    n->right = NULL;
    return n;
}

tree *tree_alloc(int (*compare)(TREE_TYPE *a, TREE_TYPE *b),
	void (*print)() ) {
    tree *T = malloc(sizeof(tree));
    T->nil = tree_node(NULL);
    T->nil->left= T->nil;
    T->nil->right= T->nil;
    T->root = T->nil;

    T->compare = compare;
    T->print = print;

    return T;
}

void tree_walk_inorder(tree *T, node *n){
    if (n != T->nil) {
	tree_walk_inorder(T, n->left);
	T->print(n);
	tree_walk_inorder(T, n->right);
    }
}

node * tree_search(tree *T, node *x, TREE_TYPE *k)
{
    if (x == T->nil || T->compare(x->key, k))
	return x;
    if (T->compare(k, x->key) < 0)
	return(tree_search(T, x->left, k));
    else
	return(tree_search(T, x->right, k));
}

node * tree_min(tree *T, node *x) {
    while (x->left != T->nil) {
	x = x->left;
    }
    return x;
}

node *tree_max(tree *T, node *x) {
    while(x->right != T->nil) {
	x = x->right;
    }
    return x;
}

node * tree_successor(tree *T, node *x) {
    node *y;
    if (x->right != T->nil)
	return tree_min(T, x->right);
    y = x->p;
    while (x != T->nil && x == y->right) {
	x = y;
	y= y->p;
    }
    return y;
}

void tree_insert(tree *T, node *z, int unique)
{
    node *x, *y;

    y = T->nil;
    x = T->root;

    while (x!= T->nil) {
	y = x;
	if (T->compare(z->key, x->key) < 0) {
	    x = x->left;
	} else {
	    x = x->right;
	}
    }
    z->p = y;
    if (y == T->nil) { /* tree was empty */
	T->root = z;
    } else {
	if (T->compare(z->key, y->key) < 0) {
	    y->left = z;
	} else if (T->compare (z->key, y->key) == 0 && unique){
	    return;
	} else {
	    y->right = z;
	}
    }
    z->left = T->nil;
    z->right = T->nil;
}

void tree_delete(tree *T, node *z)
{
    node *y, *x;
    if (z->left == T->nil || z->right == T->nil) {
	/* one child: splice z out of tree */
	y = z;
    } else {
	y = tree_successor(T, z);
    }
    /* y now points to victim: either original node or sucessor */
    if (y->left != T->nil) {
	x = y->left;
    } else {
	x = y->right;
    }
    /* x now set to child of y (or nil if no children */

    /* splicing.  more complicated than one might first expect due to handling
     * - "x = NIL"
     * - "y is root" */
    if (x!= T->nil) {
	x->p = y->p;
    }
    if (y->p == T->nil) {
	T->root = x;
    } else {
	if (y == y->p->left) {
	    y->p->left = x;
	} else {
	    y->p->right = x;
	}
    }

    /* copy payload if if z's successor was node spliced out */
    if (y != z) {
	z->key = y->key;
	z->value = y->value;
    }
    /* clr returns y */
    free(y);
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static int height(tree *T, node *n) {
    if (n == T->nil)
	return 0;
    return(MAX(height(T, n->left), height(T, n->right)) + 1);
}


int tree_height(tree *T) {
    return height(T, T->root);
}

static void print_helper_dot(tree *T, node *x)
{
    if (x == T->nil) return;

    if (x->left != T->nil) {
	printf("   %lld->%lld;\n", (long long)x, (long long)x->left);
	print_helper_dot(T, x->left);
    }
    printf("   %lld [label=\"", (long long)x);

    T->print(x);

    printf("\"];\n");

    if (x->right != T->nil) {
	printf("   %lld->%lld;\n", (long long)x, (long long)x->right);
	print_helper_dot(T, x->right);
    }
}

void print_helper(tree *T, node *x, int depth)
{
    int i;
    if (x == T->nil) return;
    if (x->left != T->nil)
	print_helper(T, x->left, depth+1);

    for (i=0; i<depth; i++)
	printf("-");
    T->print(x);
    printf("\n");
    if (x->right != T->nil) {
	print_helper(T, x->right, depth+1);
    }
}

void print_tree_dot(tree *t)
{
    static int counter=0;
    printf("digraph rb_tree_%d {\n", counter++);
    print_helper_dot(t, t->root);
    printf("    }\n");
}

void tree_print(tree *t, int format_flags)
{
    if (format_flags & TREE_DOT)
	print_tree_dot(t);
}
