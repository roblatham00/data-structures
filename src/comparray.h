
/* it's kind of like a growable array, in that one can continue to add elements
 * to it, but it's not like a growable array in that the data is stored compressed */

#ifndef COMPARRAY_H
#define COMPARRAY_H

#define COMPARRAY_TYPE int64_t

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

struct comparray_stat {
    size_t nbytes;  /* amount of data stored in this data structure */
    size_t cbytes;  /* amount of compressed data stored */
    size_t metabytes; /* a.k.a overhead */
};

typedef int comparray; /* there's nothing the caller can poke at */

/* initialize comparray library and any dependencies */
void comparray_init();

/* clean up */
void comparray_finalize();

/* create a comparray container that stores data of size 'type_size' in chunks
 * consisting of a 'chunk_size' number of elements */
comparray comparray_create(size_t chunk_size, size_t type_size);

/* retreive the 'count' elements from compressed array 'array' at index 'index' */
COMPARRAY_TYPE * comparray_get_n(comparray array, int64_t index, int64_t count);

/* given an array 'value' of 'count' items, store in compressed array 'array'
 * at index 'index' */
int comparray_set_n(comparray array, int64_t index, int64_t count, COMPARRAY_TYPE *value);

/* deallocate everything associated with 'id' */
void comparray_free(comparray id);

/* need a 'set index' and 'next n' */

COMPARRAY_TYPE * comparray_next_n(comparray array, int64_t count);

int comparray_seek(comparray array, int64_t index, int whence);

/* dump out the internal representation of the array */
void comparray_display(comparray array);

/* some rudimentary statistics about the data structure */
int comparray_stat(comparray array, struct comparray_stat *cstats);
#endif
