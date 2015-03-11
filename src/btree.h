#ifndef BTREE_H
#define BTREE_H

#ifndef BTREE_TYPE
#define BTREE_TYPE void
#endif

typedef enum {
    INTERNAL,
    LEAF
} leaf;

typedef struct {
    int n;                /* number of keys */
    BTREE_TYPE *keys;     /* keys stored in nondecreasing order */
    btree_node *children; /*  n + 1 children. leaf nodes have no children and
			      are undefined */
    leaf is_leaf;         /* boolean: true if leaf, false if internal */
} btree_node;

typedef struct {
    btree_node *root;
    void *compressed_stream;
    int (*compare)(BTREE_TYPE *a, BTREE_TYPE *b);
    void decompress();
} btree


typedef struct {
    btree_node *node;
    int index;
} btree_search_result;

/* btree_search:
 *   input: takes a ponter to the root node 'x' of a subtree and a key 'k' to
 *          search for in that subtree
 *   output: returns node 'y' and index 'i': such that y->keys[i] is k.
 *           NIL for no match */
btree_search_result *btree_search(btree *T, btree_node *x, BTREE_TYPE *k);
btree_create(btree T);
btree_insert();

