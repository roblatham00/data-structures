ACLOCAL_AMFLAGS="-Im4"

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
lib_LTLIBRARIES += libcomparray.la
libcomparray_la_SOURCES = src/blockcache.c\
			  src/comparray.c

include_HEADERS += src/comparray.h

noinst_HEADERS = src/blockcache.h \
		 src/comparray.h \
		 src/interval_tree.h \
		 src/orderstat.h \
		 src/rbtree.h

#
# skiplists
#
lib_LTLIBRARIES += libskiplist.la
libskiplist_la_SOURCES = src/skiplist.c

include_HEADERS += src/skiplist.h

# testing
check_PROGRAMS = test/test-comparray \
		 test/test-interval \
		 test/test-ordered \
		 test/test-rb \
		 test/test-skiplist


check_CPPFLAGS = -I$(srcdir)/src

TESTS = test/test-comparray \
	test/test-interval \
	test/test-ordered \
	test/test-rb \
	test/test-skiplist

test_test_comparray_CPPFLAGS = -I${srcdir}/src
test_test_comparray_LDADD =  -lcomparray -lrbtree -lblosc

test_test_interval_CPPFLAGS = -I${srcdir}/src
test_test_interval_LDADD = -lrbtree

test_test_ordered_CPPFLAGS = -I${srcdir}/src
test_test_ordered_LDADD = -lrbtree

test_test_rb_CPPFLAGS = -I${srcdir}/src
test_test_rb_LDADD = -lrbtree

test_test_skiplist_CPPFLAGS = -I${srcdir}/src
test_test_skiplist_LDADD = -lskiplist


# documentation products
dist_doc_DATA = README.md

DISTCHECK_CONFIGURE_FLAGS = CPPFLAGS=${CPPFLAGS}  LDFLAGS=${LDFLAGS}

# coverage requires lcov
coverage-report:
	lcov --directory . --capture --output-file coverage.info
	genhtml --legend --output-directory coverage coverage.info

