#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef RBTREE_TYPE
#define RBTREE_TYPE void
#endif
#include "rbtree.h"

int int_compare(RBTREE_TYPE *a, RBTREE_TYPE *b)
{
    int x = *((int *)a);
    int y = *((int *)b);

    if (x<y) return -1;
    if (x==y) return 0;

    return 1;
}

int string_compare(void *a, void *b)
{
    return strcmp(a, b);
}

void int_print(RBTREE_TYPE *a, void *value) {
    printf("%d", *(int *)a);
}
rb_node *mknode(int i)
{
    int *key = malloc(sizeof(*key));
    *key = i;
    rb_node *z = rb_new_node(key);
    return z;
}

/* throw a bunch of random values at a tree */
void tree_test_random(int nr_nodes)
{
    int i;
    rb_tree * tree = rb_new_tree(int_compare, int_print);

    for (i=0; i< nr_nodes ; i++) {
	rb_insert(tree, mknode(random()) );
    }
    rb_print_tree(tree, RB_TREE_DOT);
    return;
}

static void shuffle(int n, int *array)
{
    /* fisher-yates shuffle */
    int i, j;
    for(i=(n-1); i> 0; i--) {
       int t;
       j = (rand() % i);
       t = array[i];
       array[i] = array[j];
       array[j] = t;
    }
}

void tree_test_delete_random(int nr_nodes)
{
    int i;
    int *targets = malloc(sizeof(*targets)*nr_nodes);
    rb_node *d;
    rb_tree *tree = rb_new_tree(int_compare, int_print);

    for (i=0; i< nr_nodes ; i++) {
	targets[i] = random();
	rb_insert(tree, mknode(targets[i]));
    }

    shuffle(nr_nodes, targets);

    for (i=0; i< nr_nodes ;i++ ) {
	d = rb_search(tree, &(targets[i]));
	if (d != tree->nil) {
	    d = rb_delete(tree, d);
	    free(d);
	}
    }
    rb_print_tree(tree, RB_TREE_DOT);

}

void tree_test_delete()
{
    rb_tree *tree = rb_new_tree(int_compare, int_print);
    rb_insert(tree, mknode(3));
    rb_insert(tree, mknode(1));
    rb_insert(tree, mknode(5));
    rb_insert(tree, mknode(2));
    rb_insert(tree, mknode(7));


    /* try to delete a node that does not live on the tree */
    rb_node *d;

    d = rb_delete(tree, mknode(3));
    free(d);

    /* we now have a 5-node tree with red leaves 2 and 7 */
    int key = 1;
    d = rb_search(tree, &key);
    if (d != tree->nil) {
	rb_delete(tree, d);
	free(d);
    }

}
int main(int argc, char **argv)
{
    tree_test_random(atoi(argv[1]));

    tree_test_delete();

    tree_test_delete_random(atoi(argv[1]));

    return 0;
}
