#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "comparray.h"
#include "blockcache.h"
#include "interval_tree.h"

/* not entirely sure which level of abstraction layer this should go */
#define DEFAULT_CHUNK_SIZE 10

#define AS_INT64(p) (*((int64_t *)p))


typedef struct {
    int64_t last ;           /* end of array */
    int64_t chunk_size;     /* size of each block (before compression) */
    int64_t index;          /* internal placholder 'cursor' for the streaming
			       operations */
    size_t typesize;        /* size of basic element stored in array */
    blockcache_item *cache;
    interval_tree *blocks;
} comparray_internal;

#define MAX_COMPARRAYS 5
static comparray_internal * internal_arrays[MAX_COMPARRAYS];

int block_compare(BLOCKCACHE_TYPE * a, BLOCKCACHE_TYPE *b)
{
    /* compare-by-subtraction might be too clever if numbers are so far apart as to overflow int.  some other approaches:
     *  return (va > vb) - (va < vb);
     *  return (va < vb? -1 : va > vb? 1 : 0)
     * see http://stackoverflow.com/questions/6103636/c-qsort-not-working-correctly */
    return (*(blkcache_idx_t*)a- *(blkcache_idx_t *)b);
}

void block_free(rb_node *a)
{
    /* TODO: think about what's stored in this tree */
    free(a->value);
    free(a->low);
    free(a->high);
}

void block_print(rb_node *n)
{
    printf("%ld: [%ld-%ld] (%ld)", *(int64_t *)n->key,
	    *((int64_t *)n->low), *((int64_t *)n->high), *((int64_t *)n->max));
}

void comparray_init()
{
    blockcache_init();
}
void comparray_finalize()
{
    blockcache_finalize();
}

comparray comparray_create(size_t chunk_size, size_t type_size)
{
    comparray_internal *carray;

    carray = malloc(sizeof(comparray_internal));
    carray->index = 0;
    carray->last = 0;
    carray->chunk_size = chunk_size;
    carray->typesize = type_size;
    /* tree is empty: no blocks held here */
    carray->blocks = rb_new_tree(block_compare, block_free, block_print);
    carray->cache = malloc(sizeof(blockcache_item));
    blkcache_idx_t *low, *high;

    /* inital state: a single cached block of zeros, but nothing in the backing
     * tree */
    carray->cache->data = calloc(chunk_size, type_size);
    low = malloc(sizeof(*low));
    high = malloc(sizeof(*low));
    *low = 0;
    *high = chunk_size-1;
    carray->cache->node = interval_new_node(low, high);

    int i;
    for (i=0; i< MAX_COMPARRAYS; i++ ) {
	if (internal_arrays[i] == 0) break;
    }
    internal_arrays[i] = carray;
    return i;
}

void comparray_free(comparray array)
{
    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return;

    rb_delete(carray->blocks, carray->cache->node);
    rb_delete_tree(carray->blocks);
    free(carray->cache->data);
    free(carray->cache);
    free(carray);
    internal_arrays[array] = NULL;
}

COMPARRAY_TYPE * comparray_get_n(comparray array, int64_t index, int64_t count)
{
    char *value;
    int ret;

    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return NULL;

    value = malloc(count*carray->typesize);
    ret = blockcache_get(carray->cache, carray->blocks,
	    index, count, (COMPARRAY_TYPE *)value,
	    carray->chunk_size, carray->typesize);
    if (ret != COMPARRAY_OK) {
	free (value);
	return NULL;
    }
    return (COMPARRAY_TYPE *)value;
}

int comparray_fill_n(comparray array, int64_t index,
	int64_t count, void *dest)
{
    int ret;
    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return -1;

    /* just like set_n, excpt we fill up the user-provided buffer instead of
     * allocating our own.  Probably should have just done it this way in the
     * first place */
    ret = blockcache_get(carray->cache, carray->blocks,
	    index, count, (COMPARRAY_TYPE *) dest,
	    carray->chunk_size, carray->typesize);
    return ret;
}


int comparray_set_n(comparray array, int64_t index,
	int64_t count, COMPARRAY_TYPE *value)
{
    int ret;
    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return COMPARRAY_INVALID;

    /* we cannot operate directly on compressed data: that's the whole point of
     * this data structure.  instead, we operate on a cached block  */
    ret = blockcache_set(carray->cache, carray->blocks,
	    index, count, value, carray->chunk_size, carray->typesize);

    if (ret == 0) return COMPARRAY_OK;
    return ret;
}

/* mostly for debugging */
void comparray_display(comparray array)
{
    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return;

    rb_print_tree(carray->blocks, RB_TREE_DOT);
}

int comparray_stat(comparray array, struct comparray_stat *cstats)
{
    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return 0;

    blockcache_flush(carray->cache, carray->blocks, carray->typesize);
    /* the bookeeping items in the block cache */
    cstats->metabytes=sizeof(*carray) + sizeof(blockcache_item) +
	(carray->chunk_size*carray->typesize) + sizeof(interval_node);
    /* nothing compressed in the cache */
    cstats->nbytes=0;
    cstats->cbytes=0;
    /* then inorder traverse the tree looking at the compressed items */
    blockcache_stat_helper(carray->blocks, carray->blocks->root, cstats);
    return 0;

}
void comparray_zap(void *p)
{
    free(p);
}
