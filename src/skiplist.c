#include <stdlib.h>
#include <limits.h>
#include "skiplist.h"
#include <assert.h>

/*
 * P: a fraction 'p' of the nodes with level i pointers also have level i+1
 * pointers.  The Pugh paper gives .5
 */
double P = 0.5;

/*
 * special sentinal nodes
 */

skiplist_node nil = {
    .key = NULL,
    .value = NULL,
    .level = 0,
    .forward = NULL,
};

skiplist_node neginf = {
    .key = 0,
    .value = NULL,
    .level = 0,
    .forward = NULL,
};

/* uh, obviously not actually infinity.  sentiel */
int positive_infinity = INT_MAX;
skiplist_node posinf = {
    .key = &positive_infinity,
    .value = NULL,
    .level = 0,
    .forward = NULL,
};


/*
 * utility functions internal to skiplist
 */

int random_level(skiplist *L) {
    int level = 1;
    while (random()/RAND_MAX < P && level < L->max_level)
	level = level + 1;
    return level;
}

skiplist_node * make_node(int level, SKIPLIST_TYPE *key, void *value)
{
    skiplist_node *dummy;
    dummy = malloc(sizeof(*dummy));
    dummy->key = key;
    dummy->value = value;
    dummy->forward = malloc(sizeof(dummy->forward)*level);

    return dummy;
}

/*
 * implementations of public functions
 */
skiplist * skiplist_new(int max_level) {

    skiplist *list;
    list = malloc(sizeof(*list));
    list->nil = &nil;
    list->header = &nil;
    list->level = 1;

    return list;
}

void * skiplist_search(skiplist *L, SKIPLIST_TYPE *k)
{
    int i;
    skiplist_node *x = L->header;
    for (i = L->level; i>0; i--) {
	/* loop invariant: x->key < searchKey */
	assert(L->compare(x->key, k) < 0);
	/* traverse forward pointers until we overshoot */
	while (L->compare(x->forward[i]->key, k) < 0)
	    x = x->forward[i];
	/* gone as far as we can on this level.  next iteration will drop down
	 * a level and repeat */
    }
    /* x->key < searchKey <= x->forward[1]->key */
    assert (L->compare(x->key, k) < 0);
    assert (L->compare(k, x->forward[1]->key) <= 0);

    /* either we are in front of the desired node, or it's not in the list */
    x = x->forward[1];
    if (L->compare(x->key, k) == 0)
	return (x->value);
    else
	return NULL;
}

void skiplist_insert(skiplist *L, SKIPLIST_TYPE *key, void *value)
{
    skiplist_node **update; /* once we find place to insert new node, will
			       splice node into skiplist by consulting pointers
			       saved in update[] vector */
    skiplist_node *x;
    int i, level;

    update = calloc(L->max_level, sizeof(*update));

    x = L->header;
    for(i= L->level; i > 1; i--) {
	while(L->compare(x->forward[i]->key, key) < 0) {
	    x = x->forward[i];
	}
	/* x->key < searchKey <= x->forward[i]->key */
	assert(L->compare(x->key, key) < 0);
	assert(L->compare(key, x->forward[i]->key) <= 0);
	/* save the forward pointers in each level. We'll need them when
	 * inserting a new item */
	update[i] = x;
    }
    x = x->forward[1];
    if (L->compare(x->key, key) == 0) {
	/* found a list node with the desired key; overwrite value */
	free(x->value);
	x->value = value;
    } else {
	/* no node matching key.  Create new node */
	level = random_level(L);
	/* insertion generated a node with level greater than previous largest
	 * level */
	if (level > L->level) {
	    for (i= L->level + 1; i<=level; i++) {
		update[i] = L->header;
	    }
	    /* update maximum level of the list */
	    L->level = level;
	}
	x = make_node(level, key, value);
	for (i = 1; i<= level; i++ ) {
	    x->forward[i] = update[i]->forward[i];
	    update[i]->forward[i] = x;
	}
    }
}
