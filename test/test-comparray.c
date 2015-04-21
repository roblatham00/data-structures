#include <stdlib.h>
#include <stdio.h>
#include "comparray.h"

void shuffle_int64t(int n, int64_t *array)
{
    int i, j;
    srand(18);
    for (i=(n-1); i> 0; i--) {
	int64_t t;
	j = (rand() % i);
	t = array[i];
	array[i] = array[j];
	array[j] = t;
    }
}

int array_compare(int count, int64_t * a, int64_t *b, char *desc)
{
    int i, nr_errors=0;

    for (i=0; i<count; i++) {
	if (a[i] != b[i]) {
	    nr_errors++;
	    printf("%s : a[%d] = %ld  b[%d] = %ld\n",
		desc, i, a[i], i, b[i]);
	}
    }
    return nr_errors;
}

int test_sizes(comparray array, size_t num_items, size_t typesize)
{
    int nr_errors = 0;
    /* space efficiecy of representation */
    struct comparray_stat cstats;
    comparray_stat(array, &cstats);
    if (cstats.nbytes != num_items * typesize) {
	fprintf(stderr, "incorrect size\n");
	nr_errors++;
    }
    return nr_errors;
}


int main(int argc, char **argv)
{
#define NR_ITEMS 1000000
#define CHUNK_SIZE NR_ITEMS/100
    comparray_init();
    comparray carray = comparray_create(CHUNK_SIZE, sizeof(int64_t));
    comparray carray_random = comparray_create(CHUNK_SIZE, sizeof(int64_t));

    int64_t i, *sequential_items, *random_items;
    int nr_errors=0;
    int64_t *compare_buf;

    sequential_items = malloc(NR_ITEMS*sizeof(int64_t));
    random_items = malloc(NR_ITEMS*sizeof(int64_t));

    for (i=0; i< NR_ITEMS; i++) {
	sequential_items[i] = i;
	random_items[i] = i;
    }
    shuffle_int64t(NR_ITEMS, random_items);

    /* entire array */
    comparray_set_n(carray, 0, NR_ITEMS, sequential_items);
    compare_buf = comparray_get_n(carray, 0, NR_ITEMS);

    nr_errors += array_compare(NR_ITEMS, sequential_items, compare_buf, "sequential");
    free(compare_buf);

    /* what about a specific item? */
    for (i=0; i< 10; i++) {
	size_t index = (rand() % NR_ITEMS);
	if (index + 17 >= NR_ITEMS) continue;
	compare_buf = comparray_get_n(carray, index, 17);
	nr_errors += array_compare(17, &(sequential_items[index]), compare_buf, "random item");
	free (compare_buf);
    }

    comparray_set_n(carray_random, 0, NR_ITEMS, random_items);
    compare_buf = comparray_get_n(carray_random, 0, NR_ITEMS);

    nr_errors += array_compare(NR_ITEMS, random_items, compare_buf, "random");

    nr_errors += test_sizes(carray_random, NR_ITEMS, sizeof(int64_t));

    /* should fail: accessing past end */
    int64_t *bad;
    bad = comparray_get_n(carray_random, NR_ITEMS, 1);
    if (bad != NULL) {
	fprintf(stderr, "retrieved item that should not exist\n");
	nr_errors++;
    }
    free(bad);
    free(compare_buf);
    free(sequential_items);
    free(random_items);

    comparray_display(carray_random);
    if (nr_errors == 0) printf(" No Errors\n");
    comparray_free(carray);
    comparray_free(carray_random);

    comparray_finalize();
    return nr_errors;
}
