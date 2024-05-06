ACLOCAL_AMFLAGS="-Im4"
AM_CPPFLAGS = -I${srcdir}/src

#
# several kinds of red-black tree data structures
#
lib_LTLIBRARIES = librbtree.la
librbtree_la_SOURCES = src/rbtree.c \
		       src/orderstat.c \
		       src/interval_tree.c

include_HEADERS = src/rbtree.h \
		  src/orderstat.h \
		  src/interval_tree.h

#
# compressed array.
#
if COMPARRAY
lib_LTLIBRARIES += libcomparray.la
libcomparray_la_SOURCES = src/blockcache.c\
			  src/comparray.c

include_HEADERS += src/comparray.h

noinst_HEADERS = src/blockcache.h \
		 src/comparray.h \
		 src/interval_tree.h \
		 src/orderstat.h \
		 src/rbtree.h
endif

#
# skiplists
#
lib_LTLIBRARIES += libskiplist.la
libskiplist_la_SOURCES = src/skiplist.c

include_HEADERS += src/skiplist.h

#
# simple tree
#
lib_LTLIBRARIES += libtree.la
libtree_la_SOURCES = src/tree.c

include_HEADERS += src/tree.h

#
# heap, heapsort, priority queue
#
lib_LTLIBRARIES += libheap.la
libheap_la_SOURCES = src/heap.c

include_HEADERS += src/heap.h\
		   src/heap-c.h

# testing
check_PROGRAMS = test/test-interval \
		 test/test-ordered \
		 test/test-rb \
		 test/test-skiplist \
		 test/test-euler-29 \
		 test/test_heap \
		 test/test_heap-c
if COMPARRAY
check_PROGRAMS += test/test-comparray
endif


check_CPPFLAGS = -I$(srcdir)/src

TESTS = test/test-interval \
	test/test-ordered \
	test/test-rb \
	test/test-skiplist \
	test/test-euler-29 \
	test/test_heap \
	test/test_heap-c

if COMPARRAY
TESTS += test/test-comparray

test_test_comparray_LDADD =  -lcomparray -lrbtree -lblosc
endif

test_test_interval_LDADD = -lrbtree

test_test_ordered_LDADD = -lrbtree

test_test_rb_LDADD = -lrbtree

test_test_skiplist_LDADD = -lskiplist

test_test_euler_29_LDADD = -ltree -lm

test_test_heap_c_LDADD = -lheap

test_test_heap_SOURCES = test/test_heap.cc

# documentation products
dist_doc_DATA = README.md

DISTCHECK_CONFIGURE_FLAGS = CPPFLAGS=${CPPFLAGS}  LDFLAGS=${LDFLAGS}

# coverage requires lcov
coverage-report:
	lcov --directory . --capture --output-file coverage.info
	genhtml --legend --output-directory coverage coverage.info

