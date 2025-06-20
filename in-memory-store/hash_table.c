#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

HashTable hash_table = {
    .hashTableValues = {0},
    .size = 0,
    .capacity = MAX_HASH_TABLE_CAPACITY
};

int create_entry(char *name, char* value){
    if(hash_table.size == hash_table.capacity){
        printf("hash table is full");
        return -2;
    }

    if(!name || !value){
        perror("name or value not found (create entry)");
        return -1;
    }

    // get the index
    int index = get_hash_table_index(name);
    printf("INDEX FOR THE NAME: %d\n", index);

    HashTableEntry *entry = &hash_table.hashTableValues[index];
    HashTableEntry *prev = NULL;

    // update the value if a name exists already
    while (entry && entry->name != NULL)
    {
        if(strcmp(entry->name, name) == 0){
            entry->value = value;
            return 0;
        }

        prev = entry;
        entry = entry->next;
    }
    

    HashTableEntry *new_entry = malloc(sizeof(HashTableEntry));;
    if (!new_entry){
        perror("Failed to malloc for new entry (create entry)");
        return -1;
    }

    new_entry->name = name;
    new_entry->value = value;
    new_entry->next = NULL;
    
    if(prev){
        // add to the end of the linked list
        new_entry->indexInList = prev->indexInList + 1;
        prev->next = new_entry;
    } else {
        // add a fully new entry to a new row
        new_entry->indexInList = 0;
        hash_table.hashTableValues[index] = *new_entry;
    }
    
    hash_table.size += 1;
    return 0;
}

char* read_entry(char *name){
    if(!name){
        printf("name not properly received (read entry)");
        return NULL;
    }

    int index = get_hash_table_index(name);

    HashTableEntry *entry = &hash_table.hashTableValues[index];

    while(entry && entry->name != NULL){
        if(strcmp(entry->name, name) == 0){
            return entry->value;
        }

        entry = entry->next; 
    }

    perror("name doesnt exist in the hash table (read entry)");
    return NULL;
}


// testing the functions

/* int main(int argc, char const *argv[])
{
    printf("\nINITIALISED HASH TABLE\n:");
    printf("HASH TABLE CAPACITY: %d\n", hash_table.capacity);

    char *name = "stelzner";
    char *name2 = "steLzner";
    char *value = "Micheal Scott";
    char *value2 = "Mciheal Scott";

    create_entry(name , value);
    create_entry(name2 , value2);
    int index = get_hash_table_index(name);
    int index2 = get_hash_table_index(name2);

    printf("CHECKING IN MAIN IF ENTERED: \n");
    printf("TABLE INDEX: %d\n", index);
    printf("TABLE INDEX FOR 2: %d\n", index2);
    printf("TABLE NAME FOR INDEX: %s\n", hash_table.hashTableValues[index].name);
    printf("TABLE VALUE FOR INDEX: %s\n", hash_table.hashTableValues[index].value);

    printf("CHECKING IN MAIN IF READ: \n");
    printf("VALUE FOR THE NAME: %s\n", read_entry(name));
    printf("VALUE FOR THE NAME 2: %s\n", read_entry(name2));

    return 0;
} */
