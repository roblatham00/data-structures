# Data Strucutres 

Sometimes it's not clear what the reuse rules are for code we find on the
internet.  I've implemented some C data structures and package them up for work.

* rb\_tree: A classic red-black tree as described in "Introduction to Algorithims" by CLRS
* ordered-statistics: An augmneted red-black tree implementing rank operations
* interval-tree:  A further-augmneted red-black tree for closed intervals

I've continued to build on this collection with "Compressed Arrays", which
provide (as best one can in C) array-like access to compressed data

## Building

Edit Makefile for your environment.  (TODO: set this up with configure and pkg-config)

    $ make
    $ make PREFIX=/path/for/installation install
    $ make PREFIX=/path/for/installation test

## API

routines for the data structures implemented in this library.  You are probably
better off looking at the header files.

### rbtree

First thing to do is create an empty tree.  The "compare" function acts like
strcmp, returning negative if a is less than b, 0 if equal, positive if a > b.
The print function turns your key and value into something human-readable.  Primarily
used for rb\_print\_tree but also useful for debugging.

    rb_tree * rb_new_tree(int (*compare)(void *a, void *b),
    	void (*print)(void *key, void *value));

A new node in the tree has some blob of memory 'key'.  Your compare function will know how to cast/compare this blob

    rb_node * rb_new_node(void *key);

Once you create a node, instert it into the tree.

    void rb_insert(rb_tree *t, rb_node *z);

This routine deletes the node from the tree and deallocates memory associtaed with it (through the deallocation function passed to tree creation).

    void rb_delete(rb_tree *T, rb_node *z);

Print tree.  'format\_flags' can be ORed together to produce several kinds of output:

    void rb_print_tree(rb_tree *t, int format_flags);

Here are the flags:

 * RB\_TREE\_DOT: will produce [graphviz](www.graphviz.org) output
 * RB\_TREE\_DEFAULT: will produce plain text

#### Debugging

void * everywhere makes the limited C type checking even less useful.  For a
bit of help, one can define RBTREE\_TYPE before including rbtree.h.  The type
signature for the compare and print functions then take RBTREE\_TYPE * instead
of void * .

### interval tree

It's a trick! the red-black tree is already an interval tree, where every node
has a one-element interval (e.g. [2-2]).

### ordered statistics tree

Same trick.  We maintain enough additional information in the standard red
black tree to also offer ordered-statistics methods

### compressed array
what if you wanted to store ten billion items in memory?  Traditionally, one
would use an out-of-core approach, possibly with compression.  Maybe we can
keep everything in-core if we compress it.

#### related work
- growable arrays
- compressed datatypes (but not in the way that databases do it: we're
  compressing the memory representation, not on-disk)

## Author

Rob Latham (robl@mcs.anl.gov)
