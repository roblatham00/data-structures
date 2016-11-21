#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "interval_tree.h"

/* assumes that key of node and the types of the high/low endpoints are all the
 * same */
int overlaps(interval_tree *T, void *i_low, void *i_high, void *low, void * high) {
    int ret = 0;

    /* i and i' overlap if:
     *    low[i] <= high[i'] and low[i'] <= high[i] */
    if (T->compare(i_low, high) <= 0 && T->compare(low, i_high) <= 0)
	ret = 1;
    return ret;
}


/* aloocate a node (red-black in this implementation) storing an interval with
 * 'low' and 'high' but also a 'max': the maximum value of any interval
 * endpoint stored at the subtree rooted at X (CLRS: introduction to
 * algorirhms) */
interval_node *interval_new_node(INTERVAL_TYPE *low, INTERVAL_TYPE *high)
{
    rb_node *n = rb_new_node(low);
    n->low  = low;
    n->high = high;
    n->max  = high;

    return n;
}

/* Find a node in tree T whose interval overlaps [low, high].  If there is no such
 * interval, return nil */
interval_node * interval_search(interval_tree *T,
	INTERVAL_TYPE *low, INTERVAL_TYPE *high)
{
    rb_node *x;

    x = T->root;

    while ( (x != T->nil) && !(overlaps(T, low, high, x->low, x->high)) ) {

	if ( (x->left != T->nil) &&
		(T->compare( x->left->max, low) >= 0) ) {
	    x = x->left;
	} else {
	    x = x->right;
	}
    }
    return x;
}

void interval_delete(interval_tree *T, interval_node *x)
{
    rb_delete(T, x);
}

