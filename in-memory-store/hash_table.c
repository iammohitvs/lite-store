#include <stdio.h>
#include <string.h>

#include "utils.h"

HashTable hash_table = {
    .hashTableValues = {0},
    .size = 0,
    .capacity = MAX_HASH_TABLE_CAPACITY
};



int main(int argc, char const *argv[])
{
    printf("\nINITIALISED HASH TABLE\n:");
    printf("HASH TABLE CAPACITY: %d\n", hash_table.capacity);
    return 0;
}
