#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rbtree.h"

int int_compare(rb_node *a, rb_node *b)
{
    int x, y;

    x = *((int*)a->key);
    y = *((int*)b->key);

    if (x<y) return -1;
    if (x==y) return 0;

    return 1;
}

int string_compare(rb_node *a, rb_node *b)
{
    return strcmp(a->key, b->key);
}

void int_print(rb_node *a) {
    printf("%d", *(int*)a->key);
}

int main(int argc, char **argv)
{
    int i;
    rb_tree * tree = rb_new_tree(int_compare, int_print);

    for (i=0; i<atoi(argv[1]); i++) {
	int *key = malloc(sizeof(*key));
	*key = i;
	rb_node *z = rb_new_node(key);
	rb_insert(tree, z);
	if (i%3 == 0) {
	    rb_node *d = rb_delete(tree, z);
	    free(d->key);
	    free(d);
	}
    }
    rb_print_tree(tree, RB_TREE_DOT);
    return 0;
}
