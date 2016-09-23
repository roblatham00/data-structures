#include <stdio.h>
#include <stdlib.h>
#include "skiplist.h"

int int_compare(SKIPLIST_TYPE *a, SKIPLIST_TYPE* b)
{
    return *(int *)a-*(int *)b;
}
int main(int argc, char **argv)
{
    int i, *j, errs=0;
    /* does it catch fire */
    skiplist *list = skiplist_new(4, int_compare);
    i = 4;
    skiplist_insert(list, &i, NULL);
    skiplist_delete(list, &i);
    if (skiplist_search(list, &i) != NULL)
	errs++;

    for(i=0; i<10; i++) {
	j = malloc(sizeof(*j));
	*j = i;
	skiplist_insert(list, j, NULL);
    }
    i=5;
    skiplist_node *q = skiplist_search(list, &i);
    if (q == NULL)
	errs++;
}
