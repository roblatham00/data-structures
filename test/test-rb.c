#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rbtree.h"

int int_compare(void *a, void *b)
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

void int_print(void *a) {
    printf("%d", *((int*)a));
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
