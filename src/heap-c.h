#include <stdint.h>

typedef struct {
    int64_t *A;
    int64_t length;
    int64_t heap_size;
    int64_t reserve;
} Heap;


Heap * heap_create(int64_t *A, int64_t count);
void heap_destroy(Heap *h);
void heap_print(Heap *h);
void heap_sort(Heap *h);

/* priority queues */
int64_t heap_insert(Heap *h, int64_t x);
int64_t heap_maximum(Heap *h);
int64_t heap_extract_max(Heap *h);
int64_t heap_increase_key(Heap *h, int64_t x, int64_t k);
int64_t heap_insert_max(Heap *h, int64_t key);
