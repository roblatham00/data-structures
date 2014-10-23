# Data Strucutres 

Sometimes it's not clear what the reuse rules are for code we find on the
internet.  I've implemented some C data structures and package them up for work.

* rb\_tree: A classic red-black tree as described in "Introduction to Algorithims" by CLRS

## Building

Edit Makefile for your environment.  (TODO: set this up with configure and pkg-config)

    $ make
    $ make PREFIX=/path/for/installation install

## API

routines for the data structures implemented in this library.  You are probably
better off looking at the header files.

### rbtree

First thing to do is create an empty tree.  The "compare" function acts like
strcmp, returning negative if a is less than b, 0 if equal, positive if a > b.
The print function turns your "key" into something human-readable.  Primarily
used for rb_print_tree but also useful for debugging.

    rb_tree * rb_new_tree(int (*compare)(rb_node *a, rb_node *b), void (*print)(rb_node *a));

A new node in the tree has some blob of memory 'key'.  Your compare function will know how to cast/compare this blob

    rb_node * rb_new_node(void *key);

Once you create a node, instert it into the tree.

    void rb_insert(rb_tree *t, rb_node *z);

This routine deletes the node from the tree.  Caller then must free the memory associated with node

    rb_node *rb_delete(rb_tree *T, rb_node *z);

Print tree.  'format\_flags' can be ORed together to produce several kinds of output:

    void rb_print_tree(rb_tree *t, int format_flags);

Here are the flags:

 * RB\_TREE\_DOT: will produce [graphviz](www.graphviz.org) output
 * RB\_TREE\_DEFAULT: will produce plain text


## Author

Rob Latham (robl@mcs.anl.gov)
