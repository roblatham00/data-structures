#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INTERVAL_TYPE int

#include "interval_tree.h"

int int_compare(INTERVAL_TYPE *a, INTERVAL_TYPE *b) {
    int x = *(int *)a;
    int y = *(int*)b;

    return (x-y);
}

void int_print(rb_node *n) {
    printf("%d: [%d-%d] (%d)", *(int *)n->key,
	    *((int *)n->low), *((int *)n->high), *((int *)n->max));
}

void test_tree1()
{
    int j;
    /* figure 14.4 CLRS "Introduction to algorithims" */
    int intervals[][2] = {{ 16,21}, {8,9}, {25,30}, {5,8}, {15,23},
	{17,19}, {26,26}, {0,3}, {6,10}, {19,20}, {0,0} };

    interval_tree *tree = rb_new_tree(int_compare, int_print);

    for (j=0; (intervals[j][0]+intervals[j][1] != 0); j++) {
	interval *i = malloc(sizeof(*i));
	i->low = &(intervals[j][0]);
	i->high = &(intervals[j][1]);

	interval_node *n = interval_new_node(i);
	rb_insert(tree, n);
    }

    /* unsuccessful search */
    int low=11, high=14;
    interval i = {
	.low = &low,
	.high = &high,
    };

    interval_node *node = interval_search(tree, &i);
    assert(node == tree->nil);

    /* successful search */
    low=22;
    high=25;
    node = interval_search(tree, &i);
    assert( *(int*)node->low == 15);
    assert( *(int*)node->high == 23);


    /* deletion */
    node = interval_delete(tree, node);

    low=25;
    high=30;
    node = interval_search(tree, &i);
    node = interval_delete(tree, node);
    assert(*(int *)tree->root->max == 26);

    rb_print_tree(tree, RB_TREE_DOT);

}

int main(int argc, char **argv)
{
    test_tree1();
    return 0;
}

