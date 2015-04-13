#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blosc.h"
#include "interval_tree.h"
#include "blockcache.h"


#define ROUND_DOWN(x, y) ( ((x)/(y))*(y) )

typedef enum {
    BLOCKCACHE_GET,
    BLOCKCACHE_SET,
} blockcache_fetch_op;

int blockcache_init()
{
    blosc_init();

    return 0;
}


int in_cache(interval_node *node, size_t i)
{
    if (i <= *(BLOCKCACHE_TYPE*)(node->high) && *(BLOCKCACHE_TYPE*)(node->low) <= i)
	return 1;
    return 0;
}

/* dump the cached data back inko our tree.  Either update an existing range,
 * or add a new compressed block  */
int blockcache_flush(blockcache_item *cache, interval_tree *T, size_t typesize)
{
    /* - look for block in tree
     * - if no matching interval, add this one
     * - if we get a matching interval, update the payload */
    static int64_t max=-1;
    int64_t *low, *high;
    low = cache->node->low;
    high = cache->node->high;
    if (*high > max) max = *high;
    int64_t blocksize = *high - *low +1;
    interval_node *x = interval_search(T, cache->node->low, cache->node->high);
    if (x == T->nil) {
	/* no interval found: this must be a new region of data.  Compress it
	 * and add it to the tree */
	/* blosc doesn't provide a "how well will this compress" routine.
	 * Instead, we can allocate, compress, querry, and shrink */
	size_t max_destsize = blocksize*typesize + BLOSC_MAX_OVERHEAD;
	void *compressed_data = malloc(max_destsize);
	size_t blosc_nbytes, blosc_cbytes, blosc_blocksize;
	blosc_compress(9, 1, sizeof(int64_t),
		blocksize*typesize, cache->data,
		compressed_data, max_destsize);
	blosc_cbuffer_sizes(compressed_data, &blosc_nbytes,
		&blosc_cbytes, &blosc_blocksize);
	compressed_data = realloc(compressed_data, blosc_cbytes);
	cache->node->value = compressed_data;
	rb_insert(T, cache->node);
	assert(*(int64_t*)T->root->max == max);
	rb_print_tree(T, RB_TREE_DOT);
    }
    return 0;
}

/* here my nice tidy abstraction layers break down, but we need some place to
 * tie the interval tree holding compressed data and this cache */
int blockcache_fetch(blockcache_item *cache, interval_tree *T, int64_t index,
	int64_t blocksize, size_t typesize, blockcache_fetch_op flags)
{
    /* caller should only call this routine if a cache lookup has failed.  No
     * harm, aside from overhead, in calling it with a desired item already in
     * cache */
    int64_t *low, *high;
    if (cache->is_dirty) {
	blockcache_flush(cache, T, typesize);
    }
    interval_node *x = interval_search(T, &index, &index);
    if (x != T->nil ) {
	/* cache the found item */
	cache->node = x;
	low = cache->node->low;
	high = cache->node->high;
	assert(blocksize == *high - *low + 1);
	blosc_decompress(x->value, cache->data, blocksize*typesize);
	return 0;
    } else {
	if (flags & BLOCKCACHE_SET) {
	    /* note: we only put new items into the underlying tree when we
	     * flush the cache */
	    low = malloc(sizeof(int64_t));
	    high = malloc(sizeof(int64_t));
	    *low = ROUND_DOWN(index, blocksize);
	    *high = *low + blocksize -1;
	    x = interval_new_node(low, high);
	    cache->node = x;
	} else {
	    /* attempt to retrieve an item not stored: no entry available */
	    return -1;
	}
    }
    return 0;
}

/* retrieve from 'cache' of 'blocks' the 'count' values with a logical start at
 * 'index'.  Store these 'count' values (each with size 'typesize') in
 * user-provided buffer 'data' */
int blockcache_get(blockcache_item *cache, interval_tree *blocks,
	int64_t index, int64_t count, BLOCKCACHE_TYPE *data,
	int64_t blocksize, size_t typesize)
{
    size_t i;
    char *p = (char *)data;
    for(i=0; i<count; i++) {
	/* in write case, we always create new entries.  in the read case,
	 * however, need to handle "does not exist"  */
	if (!in_cache(cache->node, index+i) ) {
	    if (blockcache_fetch(cache, blocks, index+i,
			blocksize, typesize, BLOCKCACHE_GET) != 0) return -1;
	}
	/* XXX: optimize by coalescing multiple items */
	memcpy(p, (char *)(cache->data)+(i*typesize), typesize);
	p+=typesize;
    }
    return 0;
}

/* given a user buffer 'data' of length 'size', and a cached block, fill 'er
 * up.  Returns the amount of data tranfered.  Will stop when reaching either
 * the end of the user buffer or the end of the cached block, whichever comes
 * first */
size_t blockcache_fill(blockcache_item *cache, int64_t index, BLOCKCACHE_TYPE * data,
	int64_t count, size_t typesize)
{
    size_t nr_items = count;
    char *start;

    start = *(int64_t*)cache->node->low;
    start = data + start*typesize
}
/* blockcache_set: two cases.
 * - If block is in cache, update cached data with 'data'
 * - If block not in cache, fetch block
 * -- but, if no block exists (we haven't written that value yet), we need to
 *    create a new block in lower level data structure */
int blockcache_set(blockcache_item *cache, interval_tree *blocks, int64_t index,
	int64_t count, BLOCKCACHE_TYPE *data, int64_t blocksize, size_t typesize)
{
    size_t i;
    int64_t num_moved=0;
    int ret;
    char *src = (char*)data;

    while (num_moved < count) {
	if (!in_cache(cache->node, index+num_moved) ) {
	    ret = blockcache_fetch(cache, blocks, index+i,
		    blocksize, typesize, BLOCKCACHE_SET);
	    if (ret != 0) return -1;
	}
	num_moved += blockcache_fill(cache, data);
	/* TODO: optimize this by batching up requests.. "while in_cache..."
	 * it also makes the memcpy math a lot easier */
	memcpy((char *)(cache->data)+(i*typesize), src, typesize);
	cache->is_dirty=1;
	src+=typesize;
    }
    return 0;
}

void blockcache_finalize()
{
}
