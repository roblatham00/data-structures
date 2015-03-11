#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "comparray.h"
#include "interval_tree.h"

#include "blosc.h"

#define ROUND_DOWN(x, y) ( ((x)/(y))*(y) )
#define ROUND_UP(x, y)   ( ( ((x)+((y)-1)) / (y)) * (y))

#define AS_INT64(p) (*((int64_t *)p))

#define COMPARRAY_DEFAULT_CHUNK_SIZE 10

typedef struct {
    void * data;
    int64_t low;
    int64_t high;
} blockcache_t;
	
typedef struct {
    blockcache_t blockcache;
    int64_t size;           /* questionable: total number of items stored */
    int64_t block_size;     /* size of each block (before compression) */
    int64_t index;          /* internal placholder 'cursor' for the streaming
			       operations */
    interval_tree *blocks;
} comparray_internal;

#define MAX_COMPARRAYS 25
static comparray_internal * internal_arrays[MAX_COMPARRAYS];

/* TODO: update these tree functions to operate on "compressed block", not int64 */
int block_compare(void * a, void *b)
{
    return (*(int64_t*)a- *(int64_t*)b);
}

void block_free(rb_node *a)
{
    /* TODO: think about what's stored in this tree */
    free(a->key);
}

void block_print(rb_node *a)
{
    printf("%ld", *(int64_t *)(a->key) );
}

/* simple abstraction layer for compression.  snappy, blosc, whatever, but
 * presently only for blosc */
void compress_init() {
    blosc_init();
}

void compress_destroy() {
    blosc_destroy();
}
int compress(const void *src, size_t src_length, void *dest, size_t dest_size)
{
    return (blosc_compress(9, 1, sizeof(int64_t),
		src_length, src, dest, dest_size));
}

int decompress(const void *src, size_t src_length, void *dest, size_t dest_size)
{
    return (blosc_decompress(src, dest, dest_size));
}

void comparray_init()
{
    compress_init();
}
void comparray_finalize()
{
    compress_destroy();
}

comparray comparray_alloc()
{
    comparray_internal *carray;

    carray = malloc(sizeof(comparray_internal));
    carray->data = malloc(1024); /* not really sure yet how to handle this */
    carray->size = 0;
    carray->block_size = COMPARRAY_DEFAULT_CHUNK_SIZE;
    carray->index = 0;
    carray->blocks = rb_new_tree(block_compare, block_free, block_print);

    int i;
    for (i=0; i< MAX_COMPARRAYS; i++ ) {
	if (internal_arrays[i] == 0) break;
    }
    internal_arrays[i] = carray;
    return i;
}

COMPARRAY_TYPE * comparray_get_n(comparray array, int64_t index, int64_t count)
{
    int64_t i;

    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return NULL;

    COMPARRAY_TYPE *value, *t = malloc(count *sizeof(COMPARRAY_TYPE));
    value = carray->data;
    for (i=0; i< count; i++ ) {
	t[i] = value[index+i];
    }

    return t;
}

int comparray_set_n(comparray array, int64_t index,
	int64_t count, COMPARRAY_TYPE *value)
{
    int i;
    int64_t low, high, processed, block_off;
    COMPARRAY_TYPE *t;
    interval_node *n;

    comparray_internal *carray = internal_arrays[array];
    if (carray == NULL) return COMPARRAY_INVALID;

    /* we cannot operate directly on compressed data: that's the whole point of
     * this data structure.  instead, we operate on a cached block  */


    /* caller provides a list of values from array[index] to
     * array[index+count-1].  This list could be a full compressed block or
     * could overlap two blocks */
    low = ROUND_DOWN(index, carray->block_size);
    high = ROUND_UP(index+count, carray->block_size);

    /* do we have a block already? if so, we need to update (some) of it */
    n = interval_search(carray->blocks, &low, &high);
    if (n != NULL) {
	/* three cases: partial overlap with low, partial overlap with high,
	 * full overlap */
	block_off = AS_INT64(n->low);

    }

    /* if there is no block, create and insert one */
    for(i=0; i< count; i++) {
	t = carray->data;
	t[index+i] = value[i];
    }
    return COMPARRAY_OK;
}
