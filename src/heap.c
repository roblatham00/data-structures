#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "heap-c.h"

void build_max_heap(Heap *h);

Heap * heap_create(int64_t *A, int64_t count)
{
    Heap *tmp;
    tmp = malloc(sizeof(*tmp)+1);
    if (count > 0)
        tmp->A = malloc((count)*sizeof(int64_t));
    else
        tmp->A = NULL;
    /* zero-based heap, so 'length' and 'heap_size' should be indexes into
     * array;  we need both 'length' and 'reserve' -- 'length' indicates last
     * valid value of backing array (even if not part of the heap), while
     * 'reserve' indicates how many items are allocated (some/all of which
     * might not contain valid values) */
    tmp->length = count-1;
    tmp->heap_size = count-1;
    tmp->reserve = count;

    if (count > 0) {
        memcpy(tmp->A, A, count*sizeof(int64_t));
        build_max_heap(tmp);
    }

    return tmp;
}

void heap_destroy(Heap *h)
{
    free(h->A);
    free(h);
}

static inline int64_t parent(int64_t i)
{
    return((i-1)/2);
}

/* zero based array, not 1-based as in CLR */
static inline int64_t left(int64_t i) {
    return(2*i+1);
}

/* zero based array, not 1-based as in CLR */
static inline int64_t right(int64_t i) {
    return(2*i+2);
}


static inline void swap(int64_t *a, int64_t *b)
{
    int64_t tmp = *a;
    *a = *b;
    *b = tmp;
}
/* Heap *A: array of values
 * i:  index into Heap's array */
void max_heapify(Heap *A, int64_t i) {
    int64_t l = left(i);
    int64_t r = right(i);
    int64_t largest=0;

    if ( (l <= A->heap_size ) &&
            A->A[l] > A->A[i] )
        largest = l;
    else
        largest = i;

    if ( (r <= A->heap_size) &&
            A->A[r] > A->A[largest])
        largest = r;

    if (largest != i) {
        swap(&(A->A[i]), &(A->A[largest]));
        max_heapify(A, largest);
    }
}

/* Heap *A: array of values
 * i: inext into Heap's array */
/* like max_heapify except now must satisfy the min-heap property:
 *     A[Parent(i) <= A[i]
 *  parent of a node must be smaller than that node
 *  or, children of a node must be greater than or equal to a node */

void min_heapify(Heap *A, int64_t i) {
    int64_t l, r, smallest=-1;

    l = left(i);
    r = right(i);

    if ( (l <= A->heap_size) &&
            A->A[l] < A->A[i])
        smallest = l;
    else
        smallest = i;

    if ( (r <= A->heap_size) &&
            A->A[r] < A->A[smallest])
        smallest = r;

    if (smallest != i) {
        swap(&(A->A[i]), &(A->A[smallest]));
        min_heapify(A, smallest);
    }
}

void build_max_heap(Heap *h)
{
    h->heap_size = h->length;
    for (int64_t i= (h->length+1)/2; i >=0; i--) {
        max_heapify(h, i);
    }
}

void heap_print(Heap *h) {
    static int counter=0;
    printf("digraph heap_%d {\n", counter++);
    for (int64_t i=0; i<= h->heap_size; i++) {
        int64_t l = left(i);
        int64_t r = right(i);
        if (l <= h->heap_size)
            printf("   %ld->%ld;\n", i, l);
        if (r <= h->heap_size)
            printf("   %ld->%ld;\n", i, r);
        printf("   %ld [label=%ld, xlabel=%ld]\n", i, h->A[i], i);
    }
    printf("    Array [ shape = record, label = \"");
    for (int64_t i=0; i< h->length; i++)
        printf(" %ld |", h->A[i]);
    printf(" %ld \"]\n", h->A[h->length]);
    printf("}\n");
}

void heap_sort(Heap *h) {
    build_max_heap(h);
    for (int64_t i = h->length; i>= 1; i--) {
        swap(&(h->A[0]), &(h->A[i]) );
        h->heap_size--;
        max_heapify(h, 0);
    }
}

int64_t heap_maximum(Heap *h) {
    return h->A[0];
}

int64_t heap_extract_max(Heap *h)
{
    if (h->heap_size < 0) {
        fprintf(stderr, "heap underflow");
        return -1;
    }
    /* it's a swap, except we also hold onto the largest value */
    int64_t max = h->A[0];
    h->A[0] = h->A[h->heap_size];
    h->heap_size--;
    max_heapify(h, 0);
    return max;
}

int64_t heap_increase_key(Heap *h, int64_t i, int64_t key)
{
    if (key < h->A[i]) {
        fprintf(stderr, "new key smaller than current key");
    }
    h->A[i] = key;
    while ( i > 0 && h->A[parent(i)] < h->A[i] ) {
            swap(&(h->A[i]), &(h->A[parent(i)]) );
            i = parent(i);
    }
    return 0;
}


int64_t heap_insert_max(Heap *h, int64_t key) {
    /* 'heap_size' is an array index, but 'reserve' is a count of elements */
    if (h->heap_size >= h->reserve-1 || h->heap_size == -1) {
        h->A = realloc(h->A, ((h->reserve)*2+1)*sizeof(int64_t));
        h->reserve = ((h->reserve)*2 + 1);
    }
    h->heap_size++;
    h->length++;
    h->A[h->heap_size] = -INT64_MAX;
    heap_increase_key(h, h->heap_size, key);

    return 0;
}

