#include <stdlib.h>
#include <stdio.h>
#include "comparray.h"

int main(int argc, char **argv)
{
    comparray carray = comparray_alloc();

    int i, *sequential_items, *random_items, *zero_items;
    int *compare;

#define NR_ITEMS 100
    sequential_items = malloc(NR_ITEMS*sizeof(int));

    for (i=0; i< NR_ITEMS; i++) {
	sequential_items[i] = i;
    }
    comparray_set_n(carray, 0, NR_ITEMS, sequential_items);
    compare = comparray_get_n(carray, 0, NR_ITEMS);

    for (i=0; i<100; i+=6) {
	printf("sequential_items[%d] = %d  compare[%d] = %d\n",
		i, sequential_items[i], i, compare[i]);
    }

}
