#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include "comparray.h"

#define COMPARRAY_DEFAULT_CHUNK_SIZE 10
typedef struct {
    void *data;
    int64_t size;
    int64_t chunk_size;
    int64_t index;
} comparray_internal;

#define MAX_COMPARRAYS 25
static comparray_internal * internal_arrays[MAX_COMPARRAYS];

comparray comparray_alloc()
{
    comparray_internal *carray;

    carray = malloc(sizeof(comparray_internal));
    carray->data = malloc(1024); /* not really sure yet how to handle this */
    carray->size = 0;
    carray->chunk_size = COMPARRAY_DEFAULT_CHUNK_SIZE;
    carray->index = 0;

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
    COMPARRAY_TYPE *value, *t = malloc(count *sizeof(COMPARRAY_TYPE));
    value = carray->data;
    for (i=0; i< count; i++ ) {
	t[i] = value[index+i];
    }

    return t;
}

int comparray_set_n(comparray array, int64_t index, int64_t count, COMPARRAY_TYPE *value) {
    int i;
    COMPARRAY_TYPE *t;
    comparray_internal *carray = internal_arrays[array];

    for(i=0; i< count; i++) {
	t = carray->data;
	t[index+i] = value[i];
    }
    return COMPARRAY_OK;
}
