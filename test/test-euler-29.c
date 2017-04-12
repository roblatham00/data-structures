#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#define TREE_TYPE double
#include "tree.h"

int compare(TREE_TYPE *a, TREE_TYPE *b) {
    double x = *(double *)a;
    double y = *(double *)b;

    if (x > y) return -1;
    if (x == y) return 0;
    return 1;
}

void print(node *a) {
    printf("%.0f", *(double*)a->key);
}

node *mknode (double d)
{
    double *key = malloc (sizeof(key));
    *key = d;
    node *n = tree_node(key);
    return n;
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


int main(int argc, char **argv)
{
    int i, j;
    tree *t;
    int limit=5; /* some default */
    if (argc > 1) limit = atoi(argv[1]);
    int *exponents, *base;

    exponents=malloc(sizeof(exponents)*limit);
    base = malloc(sizeof(base)*limit);
    for (i=0; i<(limit-1); i++) {
	exponents[i] = i+2;
	base[i] = i+2;
    }
    /* a simple binary search tree will get really unbalanced unless input is
     * randomized. Interesting to compare the shuffled vs unshfulled cases,
     * though */

    shuffle(limit-1, exponents);
    shuffle(limit-1, base);

    t = tree_alloc(compare, print);
    for (i=0; i<= limit-1; i++)
	for (j=0; j< limit-1; j++)
	    tree_insert(t, mknode(pow(base[i], exponents[j])), 1);

    printf("#tree height is %d\n", tree_height(t));
    tree_print(t, TREE_DOT);
}
