#include "heap.h"
#include <string>

int main()
{
    //Heap<int> h1 = {16, 4, 10, 14, 7, 9, 3, 2, 8, 1};
    //h1.print();

    Heap<int> h2 = {4,1, 3, 2, 16, 9, 10, 14, 8, 7};
    h2.print();

    Heap<int> h3;
    h3.insert_max(4);
    h3.insert_max(1);
    h3.insert_max(3);
    h3.insert_max(2);
    h3.insert_max(16);
    h3.insert_max(9);
    h3.insert_max(10);
    h3.insert_max(14);
    h3.insert_max(8);
    h3.insert_max(7);

    h3.heapsort();
    h3.print();
}

