CC=gcc
CFLAGS=-Wall -g

TEST_CFLAGS=$(CFLAGS) -I$(PREFIX)/include
TEST_LDFLAGS=-L$(PREFIX)/lib
TEST_LDLIBS=-lrbtree

VALGRIND=valgrind -q --leak-check=full --show-reachable=yes --error-exitcode=100 --log-file=valgrind-\%p.out

all: librbtree.a

src/rbtree.o: src/rbtree.c src/rbtree.h

src/orderstat.o: src/orderstat.c src/orderstat.h src/rbtree.o

src/interval_tree.o: src/interval_tree.c src/interval_tree.h src/rbtree.o

librbtree.a: librbtree.a(src/rbtree.o) librbtree.a(src/orderstat.o) librbtree.a(src/interval_tree.o)

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

test: test-rb test-os test-interval
	$(VALGRIND) ./test-rb 100   | dot -Tpdf > test-rb.pdf
	$(VALGRIND) ./test-os       | dot -Tpdf > test-os.pdf
	$(VALGRIND) ./test-interval | dot -Tpdf > test-interval.pdf

install: all
	install -d $(PREFIX)/lib $(PREFIX)/include $(PREFIX)/doc
	install -m 644 librbtree.a $(PREFIX)/lib/
	install -m 644 src/rbtree.h $(PREFIX)/include
	install -m 644 README.md $(PREFIX)/doc
	install -m 644 src/orderstat.h $(PREFIX)/include
	install -m 644 src/interval_tree.h $(PREFIX)/include

# coverage requires lcov
coverage-report:
	lcov --directory . --capture --output-file coverage.info
	genhtml --legend --output-directory coverage coverage.info

clean:
	rm -f src/*.o *.a test/*.o test-os test-rb test-interval
