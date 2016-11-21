#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_BLOSC_H
#include <blosc.h>
#endif
#include "btree.h"

/* could extend this to maintain N nodes, but for now it's just one: makes
 * cache replacement policy easier!  */
struct node_cache_s {
    btree_node *n;
    btree_node node;
} node_cache;

/* in CLRS, this woudl be DISK_READ(x), read a page from disk into operating
 * system page cache, and rely on operating system to dispose of
 * unneeded/unused caches.  read_chunk is more general in that data may not
 * live on disk.  We also cannot rely on operating system buffer cache
 * management */
btree_node * get_node(btree_node *x) {
    if (x == node_cache.n)
	return &(node_cache.node);
    /* just use blosc instead of coming up with some compression api */
    blosc_decompress(x, &(node_cache.node), sizeof(node_cache.node));
    return &(node_cache.node);
}

set_node(btree_node *x)
{
    /* experiment with different shuffle values. must be in range of 1-255 */
    blosc_compress(9, 1, 32, sizeof(*x), x, 

btree_search_result *btree_search(btree *T, btree_node *x, BTREE_TYPE *k)
{
    int i = 1;
    btree_search_result *result;
    for (i=1; i<= x->n && T->compare(k, &(x->keys[i])) > 0; i++ )
	; /* empty loop: searching keys */
    if (i<= x->n && T->compare(k, &(x->keys[i]))) {
	    result->node = x; result->index = i;
	    return (result);
    }
    if (x->is_leaf)
	return NULL;
    else {
	return btree_search(T, T->get_node(&(x->children[i])), k);
    }
}

void btree_create(btree *T)
{
    btree_node x = malloc (sizeof(x));
    x->is_leaf = LEAF;
    x->n = 0;
    chunk = T->set_node(x);
    T->root = x;
}
