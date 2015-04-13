#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "interval_tree.h"

int int_compare(INTERVAL_TYPE *a, INTERVAL_TYPE *b) {
    int x = *(int *)a;
    int y = *(int*)b;

    return (x-y);
}
int int64_compare(void *a, void *b) {
    int64_t x = *(int64_t *)a;
    int64_t y = *(int64_t *)b;

    return (x-y);
}
void int64_print(rb_node *n) {
    printf("%ld: [%ld-%ld] (%ld)", *(int64_t *)n->key,
	    *((int64_t *)n->low), *((int64_t *)n->high), *((int64_t *)n->max));
}
void int_print(rb_node *n) {
    printf("%d: [%d-%d] (%d)", *(int *)n->key,
	    *((int *)n->low), *((int *)n->high), *((int *)n->max));
}

void int_free(rb_node *a) {
    free(a->low);
    free(a->high);
}

void test_tree1()
{
    int j;
    /* figure 14.4 CLRS "Introduction to algorithims" */
    int intervals[][2] = {{ 16,21}, {8,9}, {25,30}, {5,8}, {15,23},
	{17,19}, {26,26}, {0,3}, {6,10}, {19,20}, {0,0} };

    interval_tree *tree = rb_new_tree(int_compare, int_free, int_print);

    for (j=0; (intervals[j][0]+intervals[j][1] != 0); j++) {
	int *low = malloc(sizeof(*low));
	int *high = malloc(sizeof(*high));
	*low = intervals[j][0];
	*high = intervals[j][1];

	interval_node *n = interval_new_node(low, high);
	rb_insert(tree, n);
    }

    /* unsuccessful search */
    int low=11, high=14;

    interval_node *node = interval_search(tree, &low, &high);
    assert(node == tree->nil);

    /* successful search */
    low=22;
    high=25;
    node = interval_search(tree, &low, &high);
    assert( *(int*)node->low == 15);
    assert( *(int*)node->high == 23);


    /* deletion of leaf node */
    interval_delete(tree, node);

    low=25;
    high=30;
    node = interval_search(tree, &low, &high);
    /* deletion of an internal node */
    interval_delete(tree, node);
    assert(*(int *)tree->root->max == 26);

    rb_print_tree(tree, RB_TREE_DOT);
    rb_delete_tree(tree);

}

/* the simple set of intervals from the compressed array client */
void test_tree2()
{
    int j;
    int intervals[][2] = {{0,9}, {10,19}, {20, 29}, {30, 39}, {40, 49},
	{50, 59}, {60, 69}, {70, 79}, {80, 89}, {0,0} };

    interval_tree *tree = rb_new_tree(int_compare, int_free, int_print);
    for (j=0; (intervals[j][0]+intervals[j][1] != 0); j++) {
	int *low = malloc(sizeof(*low));
	int *high = malloc(sizeof(*high));
	*low = intervals[j][0];
	*high = intervals[j][1];

	interval_node *n = interval_new_node(low, high);
	rb_print_tree(tree, RB_TREE_DOT);
	rb_insert(tree, n);
	assert(*(int *)tree->root->max == intervals[j][1]);
    }
    rb_print_tree(tree, RB_TREE_DOT);
    rb_delete_tree(tree);
}
/* the simple set of intervals from the compressed array client */
void test_tree3()
{
    int j;
    int64_t intervals[][2] = {{0,9}, {10,19}, {20, 29},
	{0,0} };
    interval_tree *tree = rb_new_tree(int64_compare, int_free, int64_print);
    for (j=0; (intervals[j][0]+intervals[j][1] != 0); j++) {
	int64_t *low = malloc(sizeof(*low));
	int64_t *high = malloc(sizeof(*high));
	*low = intervals[j][0];
	*high = intervals[j][1];

	interval_node *n = interval_new_node(low, high);
	rb_insert(tree, n);
	rb_print_tree(tree, RB_TREE_DOT);
	assert(*(int64_t *)tree->root->max == intervals[j][1]);
    }
    rb_print_tree(tree, RB_TREE_DOT);
    rb_delete_tree(tree);
}

int main(int argc, char **argv)
{
    test_tree1();
    test_tree2();
    test_tree3();
    return 0;
}

