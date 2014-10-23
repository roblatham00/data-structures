CC=gcc
CFLAGS=-Wall -g

TEST_CFLAGS=$(CFLAGS) -I$(PREFIX)/include
TEST_LDFLAGS=-L$(PREFIX)/lib
TEST_LDLIBS=-lrbtree

librbtree.a: librbtree.a(src/rbtree.o)
all: librbtree.a src/rbtree.o src/rbtree.h

test/test-rb.o: test/test-rb.c
	$(CC) $(TEST_CFLAGS) $< -c -o $@

test-rb: test/test-rb.o
	$(CC) $< -o $@ $(TEST_LDFLAGS) $(TEST_LDLIBS)

test: test-rb

install: all
	install -d $(PREFIX)/lib $(PREFIX)/include $(PREFIX)/doc
	install -m 644 librbtree.a $(PREFIX)/lib/
	install -m 644 src/rbtree.h $(PREFIX)/include
	install -m 644 README.md $(PREFIX)/doc

clean:
	rm -f src/*.o *.a
