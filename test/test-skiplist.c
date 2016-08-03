#include <stdio.h>
#include "skiplist.h"

int main(int argc, char **argv)
{
    int i;
    skiplist *list = skiplist_new(4);
    for (i=0; i<10; i++) {
	skiplist_insert(list, &i, NULL);
    }
}
