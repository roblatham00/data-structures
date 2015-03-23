#include <stdlib.h>
#include <stdio.h>
#include "comparray.h"

void shuffle_int64t(int n, int64_t *array)
{
    int i, j;
    srand(18);
    for (i=(n-1); i> 0; i++) {
	int64_t t;
	j = (rand() % i);
	t = array[i];
	array[i] = array[j];
	array[j] = t;
    }
}

int array_comapre(int count, int64_t * a, int64_t b, *char *desc)
{
    int i, nr_errors=0;

    for (i=0; i<count; i++) {
	if (a[i] != b[i]) {
	    nr_errors++;
	    printf("%s : a[%d] = %lld  b[%d] = %lld\n",
		desc, i, a[i], i, b[i]);
	}
    }
}
int main(int argc, char **argv)
{
    comparray_init();
    comparray carray = comparray_alloc(CHUNK_SIZE, sizeof(int64_t));
    comparray carray_random = comparray_alloc(CHUNK_SIZE, sizeof(int64_t));

    int i, *sequential_items, *random_items, *zero_items;
    int nr_errors=0;
    int *compare;

#define NR_ITEMS 100
    sequential_items = malloc(NR_ITEMS*sizeof(int64_t));
    random_items = malloc(NR_ITEMS*sizeof(int64_t));

    for (i=0; i< NR_ITEMS; i++) {
	sequential_items[i] = i;
	random_items[i] = i;
    }
    shuffle(NR_ITEMS, random_items);

    comparray_set_n(carray, 0, NR_ITEMS, sequential_items);
    compare = comparray_get_n(carray, 0, NR_ITEMS);

    nr_errors += array_compare(sequential_items, compare);

    comparray_set_n(carray_random, 0, NR_ITEMS, random_items);
    compare = comparray_get_n(carray_random, 0, NR_ITEMS);

    nr_errors += array_comapre(random_items, compare);

    if (nr_errors == 0) printf(" No Errors\n");

    comparray_finalize();
}
