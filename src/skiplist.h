#define SKIPLIST_TYPE int


/* a node with 'k' forward pointers is called a 'level k node' */
typedef struct skiplist_node {
    SKIPLIST_TYPE *key;
    void *value;
    int level;
    /* a node at level i will have i forward pointers, indexed 0 through i-1 */
    struct skiplist_node **forward;
} skiplist_node;

typedef struct skiplist {
    /* forward pointers at levels 0 through 'max_level'.  forward pointers at
     * levels higher than 'level' point to NILL */
    skiplist_node **header;
    skiplist_node *nil;
    int max_level;            /* no level may ever be larger than max_level */
    int level;                /* current largest level; 0 if list is empty */
    int (*compare)(SKIPLIST_TYPE *a, SKIPLIST_TYPE *b);
} skiplist;

/* initialize an empty skiplist */
skiplist * skiplist_new(int max_level,
	int (*compare)(SKIPLIST_TYPE *a, SKIPLIST_TYPE *b));

/* returns value assosciated with key, or failure if not present */
void * skiplist_search(skiplist *L, SKIPLIST_TYPE *k);

/* associate specified key with associated value, creating new key if not
 * already present  */
void skiplist_insert(skiplist *l, SKIPLIST_TYPE *key, void *value);

/* delete specified key */
void skiplist_delete(skiplist *l, SKIPLIST_TYPE *key);

skiplist_node * skiplist_min(skiplist *l);

