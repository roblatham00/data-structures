#ifndef _INTERVAL_TREE_H
#define _INTERVAL_TREE_H

#ifndef INTERVAL_TYPE
# define INTERVAL_TYPE void
#else
# ifndef RBTREE_TYPE
#  define RBTREE_TYPE INTERVAL_TYPE
# endif
#endif
#include "rbtree.h"


typedef struct {
    INTERVAL_TYPE *low;
    INTERVAL_TYPE *high;
} interval;

typedef rb_node interval_node;
typedef rb_tree interval_tree;

interval_node *interval_new_node(interval *i);
void interval_insert(interval_tree *T, interval_node *x);
interval_node * interval_search(interval_tree *T, interval *i);
void interval_delete(interval_tree *T, interval_node *x);

#endif
