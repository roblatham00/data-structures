CC=gcc
CFLAGS=-Wall -g

TEST_CFLAGS=$(CFLAGS) -I$(PREFIX)/include
TEST_LDFLAGS=-L$(PREFIX)/lib
TEST_LDLIBS=-lrbtree

rbtree.o: rbtree.c rbtree.h

orderstat.o: orderstat.c orderstat.h rbtree.o

librbtree.a: librbtree.a(src/rbtree.o) librbtree.a(src/orderstat.o)


all: librbtree.a

test/test-rb.o: test/test-rb.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test/test-ordered.o: test/test-ordered.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test-rb: test/test-rb.o librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test-os: test/test-ordered.o librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test: test-rb test-os
	./test-rb 100 | dot -Tpdf > test-rb.pdf
	./test-os | dot -Tpdf > test-os.pdf

install: all
	install -d $(PREFIX)/lib $(PREFIX)/include $(PREFIX)/doc
	install -m 644 librbtree.a $(PREFIX)/lib/
	install -m 644 src/rbtree.h $(PREFIX)/include
	install -m 644 README.md $(PREFIX)/doc
	install -m 644 src/orderstat.h $(PREFIX)/include

# coverage requires lcov
coverage-report:
	lcov --directory . --capture --output-file coverage.info
	genhtml --legend --output-directory coverage coverage.info

clean:
	rm -f src/*.o *.a test/*.o test-os test-rb
