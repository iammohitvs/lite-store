#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

// DJB2 hash funciton
unsigned long hash_function(const char* name){
    if(!name){
        perror("name not found (hash function)");
        return 0;
    }

    unsigned long hash = 5381;

    int c;
    while((c = *name++)){
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

int get_hash_table_index(const char *name){
    if(!name){
        perror("name not found (get hash table index)");
        return -1;
    }

    unsigned long hash;
    int index;
    if((hash = hash_function(name)) != 0){
        index = hash % MAX_HASH_TABLE_CAPACITY;
    } else {
        perror("an error occured (get hash table index)");
        return -1;
    }

    return index;
}

// testing the hash funciton implementation

/* int main(){
    char* name1 = "Micheal Scott";
    char* name2 = "Dwight Schrute";

    printf("Index for hash 1: %d\n", get_hash_table_index(name1));
    printf("Index for hash 2: %d\n", get_hash_table_index(name2));

    return 0;
} */
