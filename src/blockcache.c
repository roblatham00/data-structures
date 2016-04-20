#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "blosc.h"
#include "interval_tree.h"
#include "blockcache.h"


#define ROUND_DOWN(x, y) ( ((x)/(y))*(y) )
#define MIN(x, y) ( (x) < (y) ? (x) : (y))
#define BLOCK_LOW(x) (*(blkcache_idx_t*)((x)->low))
#define BLOCK_HIGH(x) (*(blkcache_idx_t*)((x)->high))

typedef enum {
    BLOCKCACHE_GET,
    BLOCKCACHE_SET,
} blockcache_fetch_op;

int blockcache_init()
{
    blosc_init();

    return 0;
}


static inline int in_cache(interval_node *node, blkcache_idx_t i)
{
    if (i <= *(blkcache_idx_t*)(node->high) &&
	    *(blkcache_idx_t*)(node->low) <= i)
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
    blkcache_idx_t *low, *high;
    low = cache->node->low;
    high = cache->node->high;
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
    }
    return 0;
}

/* here my nice tidy abstraction layers break down, but we need some place to
 * tie the interval tree holding compressed data and this cache */
int blockcache_fetch(blockcache_item *cache, interval_tree *T,
	blkcache_idx_t index, int64_t blocksize,
	size_t typesize, blockcache_fetch_op flags)
{
    /* caller should only call this routine if a cache lookup has failed.  No
     * harm, aside from overhead, in calling it with a desired item already in
     * cache */
    blkcache_idx_t *low, *high;
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
	    low = malloc(sizeof(*low));
	    high = malloc(sizeof(*high));
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
	blkcache_idx_t index, int64_t count, BLOCKCACHE_TYPE *data,
	int64_t blocksize, size_t typesize)
{
    blkcache_idx_t i;
    int64_t cache_offset, cache_remain, xfer_cnt;
    char *dest = (char *)data;
    for(i=0; i<count; /* increment in loop */) {
	/* in write case, we always create new entries.  in the read case,
	 * however, need to handle "does not exist"  */
	if (!in_cache(cache->node, index+i) ) {
	    if (blockcache_fetch(cache, blocks, index+i,
			blocksize, typesize, BLOCKCACHE_GET) != 0) return -1;
	}
	/* see comments in blockcache_set for how we coalece and combine cache
	 * and buffer */
	cache_offset = (index+i)-BLOCK_LOW(cache->node);
	cache_remain = BLOCK_HIGH(cache->node) - BLOCK_LOW(cache->node) +1 -
	    cache_offset;
	xfer_cnt = MIN(cache_remain, count-i);
	memcpy(dest, (char *)(cache->data)+(cache_offset*typesize),
			typesize*xfer_cnt);
	cache->is_dirty=1;
	dest+=typesize*xfer_cnt;
	i+=xfer_cnt;
    }
    return 0;
}

/* blockcache_set: two cases.
 * - If block is in cache, update cached data with 'data'
 * - If block not in cache, fetch block
 * -- but, if no block exists (we haven't written that value yet), we need to
 *    create a new block in lower level data structure */
int blockcache_set(blockcache_item *cache, interval_tree *blocks,
	blkcache_idx_t index, int64_t count, BLOCKCACHE_TYPE *data,
	int64_t blocksize, size_t typesize)
{
    blkcache_idx_t i;
    int64_t cache_offset, cache_remain, xfer_cnt;
    int ret;
    char *src = (char*)data;

    /* consider a stream "1 2 3 4 5 6 7 8" and a two-item cache.
     * - slot for '1' is not in cache
     * - slots for the 0th and 1st item are loaded into cache
     * - determine there are two slots cached
     * - transfer to cache
     * - 3 now is not in cache, so repeat the process */
    for (i=0; i<count; /* increment in loop */) {
	if (!in_cache(cache->node, index+i) ) {
	    ret = blockcache_fetch(cache, blocks, index+i,
		    blocksize, typesize, BLOCKCACHE_SET);
	    if (ret != 0) return -1;
	}
	/* what if we have an array {1, 2, 3, 4, 5, 6} and want to set the 1st
	 * element (2).  need to determine our offset into the cache */
	cache_offset = (index+i)-BLOCK_LOW(cache->node);
	cache_remain = BLOCK_HIGH(cache->node) - BLOCK_LOW(cache->node) +1 -
	    cache_offset;
	/* then determine the smaller of "how many slots are in the cache" or
	 * "how many items are left to tranfer"  */
	xfer_cnt = MIN(cache_remain, count-i);
	memcpy((char *)(cache->data)+(cache_offset*typesize), src,
			typesize*xfer_cnt);
	cache->is_dirty=1;
	src+=typesize*xfer_cnt;
	i+=xfer_cnt;
    }
    return 0;
}

void blockcache_finalize()
{
    blosc_destroy();
}

/* inorder traversal, summing up our overhead */
int blockcache_stat_helper(interval_tree *T, interval_node *x,
	struct comparray_stat *cstats)
{
    size_t nbytes, cbytes, blocksize;
    if (x == T->nil) return 0;
    if (x->left != T->nil)
	blockcache_stat_helper(T, x->left, cstats);

    blosc_cbuffer_sizes(x->value, &nbytes, &cbytes, &blocksize);
    cstats->nbytes += nbytes;
    cstats->cbytes += cbytes;
    cstats->metabytes += sizeof(*x);

    if (x->right != T->nil)
	blockcache_stat_helper(T, x->right, cstats);

    return 0;
}
