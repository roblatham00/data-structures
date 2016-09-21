BLOSC=$(HOME)/work/soft/blosc-master
CC=gcc
CPPFLAGS=-I$(BLOSC)/include -Isrc
CFLAGS=-Wall -g -O2 -fPIC
LDFLAGS=-fPIC
ARFLAGS=Uvr
# coverage
#CC=gcc
#CFLAGS=-Wall -g -pg -fprofile-arcs -ftest-coverage
#LDFLAGS=--coverage -lgcov
# profiling
#CFLAGS=-Wall -g -O2 --profile
#LDFLAGS=--profile


TEST_CFLAGS=$(CFLAGS) -I$(PREFIX)/include
TEST_LDFLAGS=-L$(PREFIX)/lib -L$(BLOSC)/lib
TEST_LDLIBS=-lrbtree

VALGRIND=valgrind -q --leak-check=full --show-reachable=yes --error-exitcode=100 --log-file=valgrind-\%p.out

all: librbtree.a libcomparray.a libskiplist.a

src/rbtree.o: src/rbtree.c src/rbtree.h

src/orderstat.o: src/orderstat.c src/orderstat.h src/rbtree.o

src/interval_tree.o: src/interval_tree.c src/interval_tree.h src/rbtree.o


librbtree.a: librbtree.a(src/rbtree.o) librbtree.a(src/orderstat.o) librbtree.a(src/interval_tree.o)

src/comparray.o: src/comparray.h src/comparray.c

src/blockcache.o: src/blockcache.h src/blockcache.c

libcomparray.a: libcomparray.a(src/comparray.o) libcomparray.a(src/blockcache.o)

src/skiplist.o: src/skiplist.h

libskiplist.a: libskiplist.a(src/skiplist.o)

test/test-rb.o: test/test-rb.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test/test-ordered.o: test/test-ordered.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test/test-interval.o: test/test-interval.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test-rb: test/test-rb.o librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test-os: test/test-ordered.o librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test-interval: test/test-interval.o librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test-comparray: test/test-comparray.o libcomparray.a librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) -lcomparray -lblosc $(TEST_LDLIBS)

test-skiplist: test/test-skiplist.o libskiplist.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) -lskiplist

test: test-rb test-os test-interval test-comparray test-skiplist
	$(VALGRIND) ./test-rb 100   | dot -Tpdf > test-rb.pdf
	$(VALGRIND) ./test-os       | dot -Tpdf > test-os.pdf
	$(VALGRIND) ./test-interval | dot -Tpdf > test-interval.pdf
	$(VALGRIND) ./test-comparray
	$(VALGRIND) ./test-skiplist

install: all
	install -d $(PREFIX)/lib $(PREFIX)/include $(PREFIX)/doc
	install -m 644 librbtree.a $(PREFIX)/lib/
	install -m 644 src/rbtree.h $(PREFIX)/include
	install -m 644 README.md $(PREFIX)/doc
	install -m 644 src/orderstat.h $(PREFIX)/include
	install -m 644 src/interval_tree.h $(PREFIX)/include
	install -m 644 src/comparray.h $(PREFIX)/include
	install -m 644 libcomparray.a $(PREFIX)/lib/
	install -m 644 libskiplist.a $(PREFIX)/lib/


# coverage requires lcov
coverage-report:
	lcov --directory . --capture --output-file coverage.info
	genhtml --legend --output-directory coverage coverage.info

clean:
	rm -f src/*.o src/*.gcda src/*.gcno gmon.out *.a test/*.o test/*.gcda test/*.gcno test-os test-rb test-interval test-comparray
