#ifndef BLOCKCACHE_H
#define BLOCKCACHE_H

#define BLOCKCACHE_TYPE int64_t

#ifndef BLOCKCACHE_TYPE
#define BLOCKCACHE_TYPE void
#endif

#ifndef INTERVAL_TYPE
#define INTERVAL_TYPE BLOCKCACHE_TYPE
#endif
#include "interval_tree.h"

#include "comparray.h"

typedef enum blockcache_type_e {
    BLOCKCACHE_INT,
    BLOCKCACHE_DOUBLE,
    BLOCKCACHE_STR,
} blockcache_type;

typedef struct blockcache_item_t {
    BLOCKCACHE_TYPE * data;  /* the uncompressed version of the data held by 'node' */
    int is_dirty;            /* if data is dirty, then it needs to be
				compressed before we pick up a new node in this
				cache  */
    interval_node *node;     /* a range of values and their compressed representation */
} blockcache_item;

/* initialize the blockcache interface -- blosc, for example, has an init routine */
int blockcache_init();

/* given a buffer 'data' of 'count' items, store at 'index'  */
int blockcache_set(blockcache_item *cache, interval_tree *T,
	int64_t index, int64_t count, BLOCKCACHE_TYPE *data,
	int64_t blocksize, size_t typesize);
/* retrieve 'count' items at 'index' from cache and store into 'data' */
int blockcache_get(blockcache_item *cache, interval_tree *blocks,
	int64_t index, int64_t  count, BLOCKCACHE_TYPE *data,
	int64_t blocksize, size_t typesize);

void blockcache_finalize();

int blockcache_flush(blockcache_item *cache, interval_tree *T, size_t typesize);

int blockcache_stat_helper(interval_tree *T, interval_node *x, struct
	comparray_stat *cstats);
#endif
