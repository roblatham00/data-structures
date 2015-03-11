#include "btree.h"


btree_search_result *btree_search(btree *T, btree_node *x, BTREE_TYPE *k)
{
    int i = 1;
    btree_search_result *result;
    for (i=1; i<= x->n && T->compare(k, x->keys[i]) > 0; i++ )
	; /* empty loop: searching keys */
    if (i<= x->n && T->compare(k, x->keys[i])) {
	    result->node = x; result->index = i;
	    return (result);
    }
    if (x->is_leaf)
	return NULL;
    else {
	T->decompress(x->children[i]);
	return btree_search(T, decompressed_data, k);
    }
}
