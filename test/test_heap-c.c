#include <stdio.h>

#include "heap-c.h"

int main()
{
    /* heap sorting */
    int64_t a[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    Heap * h1 = heap_create(a, sizeof(a)/sizeof(a[0]));
    heap_sort(h1);

    /* heap-backed priority queue */
    int64_t b[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    Heap * h2 = heap_create(b, sizeof(b)/sizeof(b[0]));
    heap_increase_key(h2, 8, 15);


    Heap * h3 = heap_create(NULL, 0);
    heap_insert_max(h3, 4);
    heap_insert_max(h3, 1);
    heap_insert_max(h3, 3);
    heap_insert_max(h3, 2);
    heap_insert_max(h3, 16);
    heap_insert_max(h3, 9);
    heap_insert_max(h3, 10);
    heap_insert_max(h3, 14);
    heap_insert_max(h3, 8);
    heap_insert_max(h3, 7);

    heap_sort(h3);

    heap_destroy(h1);
    heap_destroy(h2);
    heap_destroy(h3);
}
