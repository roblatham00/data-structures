CC=gcc
CFLAGS=-Wall -g

TEST_CFLAGS=$(CFLAGS) -I$(PREFIX)/include
TEST_LDFLAGS=-L$(PREFIX)/lib
TEST_LDLIBS=-lrbtree

rbtree.o: rbtree.c rbtree.h

librbtree.a: librbtree.a(src/rbtree.o)


all: librbtree.a

test/test-rb.o: test/test-rb.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test-rb: test/test-rb.o librbtree.a
	$(CC) $(LDFLAGS) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test: test-rb

install: all
	install -d $(PREFIX)/lib $(PREFIX)/include $(PREFIX)/doc
	install -m 644 librbtree.a $(PREFIX)/lib/
	install -m 644 src/rbtree.h $(PREFIX)/include
	install -m 644 README.md $(PREFIX)/doc

# coverage requires lcov
coverage-report:
	lcov --directory . --capture --output-file coverage.info
	genhtml --legend --output-directory coverage coverage.info

clean:
	rm -f src/*.o *.a test/*.o
