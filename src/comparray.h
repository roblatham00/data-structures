
/* it's kind of like a growable array, in that one can continue to add elements
 * to it, but it's not like a growable array in that the data is stored compressed */

#ifndef COMPARRAY_H
#define COMPARRAY_H

#define COMPARRAY_TYPE int

#ifndef COMPARRAY_TYPE
#define COMPARRAY_TYPE void
#endif

enum {
    COMPARRAY_OK,
    COMPARRAY_NOMEM,
    COMPARRAY_INVALID,
    COMPARRAY_OTHER
};

enum {
    COMPARRAY_SET,
    COMPARRAY_CUR,
    COMPARRAY_END
};

typedef int comparray; /* there's nothing the caller can poke at */

comparray comparray_alloc();

/* retreive the 'count' elements from compressed array 'array' at index 'index' */
COMPARRAY_TYPE * comparray_get_n(comparray array, int64_t index, int64_t count);

/* given an array 'value' of 'count' items, store in compressed array 'array'
 * at index 'index' */
int comparray_set_n(comparray array, int64_t index, int64_t count, COMPARRAY_TYPE *value);

/* deallocate everything associated with 'id' */
void comparray_free(comparray *id);

/* need a 'set index' and 'next n' */

COMPARRAY_TYPE * comparray_next_n(comparray array, int64_t count);

int comparray_seek(comparray array, int64_t index, int whence);

#endif