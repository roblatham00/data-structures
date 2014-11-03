#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define RBTREE_TYPE int
#include "rbtree.h"

rb_node nil = {
    .color = BLACK,
    .key = NULL,
    .left = NULL,
    .right = NULL,
    .p = NULL,
    .size = 0,
    .low = NULL,
    .high = NULL,
    .max = NULL,

};

/* returns pointer to largest of three things */
void * max3(rb_tree *T, void *a, void *b, void *c)
{
    void *max = a;
    if ((max != NULL) && (b!= NULL) && T->compare(b, max) > 0)
	max = b;
    if ((max != NULL) && (c!= NULL) && T->compare(c, max) > 0)
	max = c;

    return max;
}
/* internal routines for tree maintenance */

static void left_rotate(rb_tree *T, rb_node *x)
{
    rb_node *y;

    /* set y */
    y = x->right;

    /* turn y's left subtree into x's right subtree */
    x->right = y->left;
    /* erratta corrects early printings of CLRS pseudocode: if y->left is nil,
     * then we would be messing with nil's parent. the nil fields are undefined
     * so this is normally ok, but e.g. delete_fixup expects nil's parent to be
     * unchanged */
    if (y->left != T->nil)
	y->left->p = x;

    /* link x's parent to y */
    y->p = x->p;

    if (x->p == T->nil) {
	T->root = y;
    } else {
	if (x == x->p->left) {
	    x->p->left = y;
	} else {
	    x->p->right = y;
	}
    }

    /* put x on y's left */
    y->left = x;
    x->p = y;
    /* augment: order-statistics tree */
    y->size = x->size;
    x->size = x->left->size + x->right->size + 1;

    /* augment: interval tree */
    y->max = x->max;
    x->max = max3(T, x->high, x->left->max, x->right->max);
}

static void right_rotate(rb_tree *T, rb_node *y)
{
    rb_node *x;

    /* set x */
    x = y->left;

    /* turn x's right subtree into y's left subtree */
    y->left = x->right;
    /* incorporate erata nil-check.  see left_rotate for more information */
    if (x->right != T->nil)
	x->right->p = y;

    /* link y's parent to x */
    x->p = y->p;

    if (y->p == T->nil) {
	T->root = x;
    } else {
	if (y == y->p->left) {
	    y->p->left = x;
	} else {
	    y->p->right = x;
	}
    }

    /* put y on x's right */
    x->right = y;
    y->p = x;
    /* augment: order-statistics tree */
    x->size = y->size;
    y->size = y->left->size + y->right->size + 1;

    /* augment: interval tree */
    x->max = y->max;
    y->max = max3(T, y->high, y->left->max, y->right->max);
}

/* restating what is in CLRS, a red-black tree has 5 properties:
 *  1: every node is either red or black
 *  2: the root is black
 *  3: every leaf (nil) is black
 *  4: if a node is red, then both children are black
 *  5: for each node, all paths from node to descendant leaves contain same
 *  number of black nodes.
 *
 *  fixup needed because insertion could possibly violate #2 (newly inserted
 *  red z is the root) or #4 (if z's parent is red, then that parent node now
 *  has a red child)
 */
static void rb_insert_fixup(rb_tree *T, rb_node *z)
{
    rb_node *y;
    /* insert_fixup only called after inserting a new red node */
    assert(z->color == RED);
    /* if z's parent is the root it must by definition of an rb tree be black,
     * and must remain the case throughout the loop */
    assert( (z->p == T->root) ? z->p->color == BLACK : 1);

    while(z->p->color == RED) {

	assert(z->p->color == RED);
	assert( (z->p == T->root) ? z->p->color == BLACK : 1);

	if (z->p == z->p->p->left ) {
	    y = z->p->p->right;
	    if (y->color == RED) {
		/* case 1: z's uncle y is red */
		z->p->color = BLACK;
		y->color = BLACK;
		z->p->p->color = RED;
		z = z->p->p;
	    } else {
		if (z == z->p->right) {
		    /* case 2: z's uncle y is black and z is a right child*/
		    z = z->p;
		    /* convert case 2 into case 3, fall through to case 3  */
		    left_rotate(T, z);
		}
		/* case 3: z's uncle y is black and z is a left child */
		z->p->color = BLACK;
		z->p->p->color = RED;
		right_rotate(T, z->p->p);
	    }
	} else { /* z is a right child */
	    y = z->p->p->left;
	    if (y->color == RED) {
		/* case 1, as above */
		z->p->color = BLACK;
		y->color = BLACK;
		z->p->p->color = RED;
		z = z->p->p;
	    } else {
		if (z == z->p->left) {
		    /* case 2, as above */
		    z = z->p;
		    /* convert to case 3 and fall through */
		    right_rotate(T, z);
		}
		/* case 3, above */
		z->p->color = BLACK;
		z->p->p->color = RED;
		left_rotate(T, z->p->p);
	    }
	}
    }
    T->root->color = BLACK;
}

static rb_node * tree_minimum(rb_tree *T, rb_node *x)
{
    while (x->left != T->nil)
	x = x->left;
    return x;
}

#if 0
static rb_node * tree_maximum(rb_tree *T, rb_node *x)
{
    while (x->right != T->nil)
	x = x->right;
    return x;
}
#endif

static rb_node * tree_successor(rb_tree *T, rb_node *x)
{
    rb_node *y = T->nil;
    if (x->right != T->nil)
	return tree_minimum(T, x->right);
    y = x->p;
    while ( (y != T->nil) && (x == y->right) ) {
	x = y;
	y = y->p;
    }
    return y;
}

static void print_helper_dot(rb_tree *T, rb_node *x)
{
    if (x == T->nil) return;

    if (x->left != T->nil) {
	printf("   %lld->%lld;\n", (long long)x, (long long)x->left);
	print_helper_dot(T, x->left);
    }
    printf("   %lld [label=\"", (long long)x);

    T->print(x);

    if (x->color == RED)
	printf( "\" color=\"red\"];\n");
    else
	printf( "\" color=\"black\"];\n");

    if (x->right != T->nil) {
	printf("   %lld->%lld;\n", (long long)x, (long long)x->right);
	print_helper_dot(T, x->right);
    }
}

void print_helper(rb_tree *T, rb_node *x, int depth)
{
    int i;
    if (x == T->nil) return;
    if (x->left != T->nil)
	print_helper(T, x->left, depth+1);

    for (i=0; i<depth; i++)
	printf("-");
    T->print(x);
    printf("\n");
    if (x->right != T->nil) {
	print_helper(T, x->right, depth+1);
    }
}

void print_tree_dot(rb_tree *t)
{
    printf("digraph rb_tree {\n");
    print_helper_dot(t, t->root);
    printf("    }\n");
}

void print_tree(rb_tree *t)
{
    print_helper(t, t->root, 0);
}



/* public routines begin here */

void rb_insert(rb_tree *T, rb_node *z)
{
    rb_node *y = T->nil;
    rb_node *x = T->root;

    z->size = 1; /* augment: ordered-statistics tree */
    while (x != T->nil) {
	x->size += 1; /* augment: ordered-statisics tree */
	y = x;
	if (T->compare(z->key, x->key) < 0) {
	    x = x->left;
	} else {
	    x = x->right;
	}
    }
    z->p = y;

    if (y == T->nil) {
	T->root = z;
    } else if ( T->compare(z->key, y->key) < 0) {
	y->left = z;
    } else {
	y->right = z;
    }

    z->left = T->nil;
    z->right = T->nil;
    z->color = RED;
    /* augment for interval tree:
     * if z is the root, it has no parent.  If it has a parent, we need to
     * update that parent's max .  insert_fixup might not update it if there's
     * no need for a rotation */
    if (z != T->root)
	z->p->max = max3(T, z->p->high, z->p->left->max, z->p->right->max);

    rb_insert_fixup(T, z);
}

rb_node *rb_search(rb_tree *T, RBTREE_TYPE *k)
{
    rb_node *x = T->root;
    while(x != T->nil && T->compare(k, x->key) ) {
	if (T->compare(k, x->key) < 0)
	    x = x->left;
	else
	    x = x->right;
    }
    return x;
}

/* delete_fixup called if removed node 'y' was black.  removing black node
 * might cause three problems:
 * 1: y was the root; a red child might then become new root
 * 2: if both x and y->p were red, a red node has red children
 * 3: paths containing y might now not be black enough; assign an extra black
 * to x and redistribute its blackness. */
void rb_delete_fixup(rb_tree *T, rb_node *x)
{
    rb_node *w;

    while ( (x != T->root) && x->color == BLACK) {
	if (x == x->p->left) {
	    w = x->p->right;
	    if (w->color == RED) {
		/* case 1: w (sibling of x) is red. w must have black children */
		w->color = BLACK;
		x->p->color = RED;
		left_rotate(T, x->p);
		w = x->p->right;
	    }
	    /* now that w is black, case 1 becomes case 2, 3 or 4 */
	    if ( (w->left->color == BLACK) && (w->right->color == BLACK) ) {
		/* case 2: node w is black and both of w's children are black:
		 * remove black w and maybe add it to parent (when we re-enter
		 * the loop) */
		w->color = RED;
		x = x->p;
		/* we're done. re-enter while loop */
	    } else {
		if (w->right->color == BLACK) {
		    /* case 3: w (x's sibling) is black, w's left is red, w's
		     * right is black): swap colors and rotate right */
		    w->left->color = BLACK;
		    w->color = RED;
		    right_rotate(T, w);
		    w = x->p->right;
		    /* now case 3 is transformed into case 4 */
		}
		/* case 4: x's sibling is black and w's right child is red:
		 * remove extra black on x by changing colors and rotating left
		 */
		w->color = x->p->color;
		x->p->color = BLACK;
		w->right->color = BLACK;
		left_rotate(T, x->p);
		x = T->root;
	    }
	} else {
	    /* same logic, except other side of the tree: right and left
	     * exchanged */
	    w = x->p->left;
	    if (w->color == RED) {
		/* case 2, as above */
		w->color = BLACK;
		x->p->color = RED;
		right_rotate(T, x->p);
		w = x->p->left;
	    }
	    if ( (w->right->color == BLACK) && (w->left->color == BLACK) ) {
		/* case 2, as above */
		w->color = RED;
		x = x->p;
	    } else {
		if (w->left->color == BLACK) {
		    /* case 3, as above */
		    w->right->color = BLACK;
		    w->color = RED;
		    left_rotate(T, w);
		    w = x->p->left;
		}
		/* case 4, as above */
		w->color = x->p->color;
		x->p->color = BLACK;
		w->left->color = BLACK;
		right_rotate(T, x->p);
		x = T->root;
	    }
	}
    }
    x->color = BLACK;
}


/* two adjustments:
 * - decrease the 'size' along the path to the parent
 * - adjust the 'max' along the path to the parent */
void augment_fixup(rb_tree *T, rb_node *x)
{
    if (x == T->nil) return;
    x->size -= 1;
    x->max = max3(T, x->high, x->left->max, x->right->max);
    augment_fixup(T, x->p);
}
/* returns the node to be reclaimed */
rb_node *rb_delete(rb_tree *T, rb_node *z)
{
    rb_node *y, *x;
    /* hardening case: what if z is bogus ? */
    if (z == NULL || z->left == NULL || z->right == NULL || z->p == NULL)
	return z;

    if ( (z->left == T->nil) || (z->right == T->nil) )
	/* z has at most one child: can kick it out of tree */
	y = z;
    else
	/* with two children, it is z's sucessor we kick out */
	y = tree_successor(T, z);


    /* set x to non-nil child, or nil if no children */
    if (y->left != T->nil)
	x= y->left;
    else
	x = y->right;

    /* re-wire y's parent and x, removing y from the tree */
    x->p = y->p;
    if (y->p == T->nil)
	T->root = x;
    else if (y == y->p->left)
	y->p->left = x;
    else
	y->p->right = x;

    /* above algorithim may choose sucessor of z to splice out.  in such a
     * case, copy y's key and additional data into z before returning y */
    if (y!=z) {
	/* needs to copy all the node information */
	z->key = y->key;
	z->value = y->value;
	/* augmented members: here is where I need a copy function */
	z->low = y->low;
	z->high = y->high;
	z->max = y->max;
	z->size = y->size;
    }

    /* augment, part 1: adjust metadata of removed node */
    y->size -= 1;
    y->max = NULL;
     /* augment, part 2: fix path up to parent */
    augment_fixup(T, y->p);

    /* removing a black node can distrub one or all of black-heights, adjacent
     * red-ness, and color of root.  fix up y's child x (x will be nil if y had
     * no children */
    if (y->color == BLACK)
	rb_delete_fixup(T, x);

    return y;
}


rb_node * rb_new_node(RBTREE_TYPE *key)
{
    rb_node *n;
    n = malloc(sizeof(*n));
    n->color = RED;
    n->key = key;
    n->p = NULL;
    n->left = NULL;
    n->right = NULL;
    /* ordered-statistics augment */
    n->size = 0; /* will increment by one once inserted */
    /* interval list augment */
    n->low = key;
    n->high = key;
    n->max = key;

    return n;
}

rb_tree * rb_new_tree(int (*compare)(RBTREE_TYPE *a, RBTREE_TYPE *b),
	void (*print)(rb_node *a))
{
    rb_tree * T = malloc (sizeof(rb_tree));

    /* sentinel node */
    T->nil = rb_new_node(NULL);
    T->nil->color = BLACK;
    T->nil->p = T->nil;
    T->nil->left = T->nil;
    T->nil->right = T->nil;

    T->root = T->nil;

    T->compare = compare;
    T->print = print;

    return T;
}

void rb_print_tree(rb_tree *t, int format_flags)
{
    if (format_flags & RB_TREE_DOT)
	print_tree_dot(t);
    if (format_flags & RB_TREE_DEFAULT)
	print_tree(t);
}


/* would rb_enumerate be useful?: output all the keys k such that a <= k <= b in
 * a red-black tree rooted a x */
