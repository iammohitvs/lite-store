#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "utils.h"

pthread_mutex_t write_mutex;
pthread_mutex_t read_count_mutex;
int readers_count = 0;

HashTable hash_table = {
    .hashTableValues = { NULL },
    .size = 0,
    .capacity = MAX_HASH_TABLE_CAPACITY
};

char* create_entry(char *name, char* value){
    char *error_return_400 = "400";
    char *full = "full";

    if(hash_table.size == hash_table.capacity){
        fprintf(stderr, "hash table is full (create entry)\n");
        return full;
    }

    if(!name || !value){
        fprintf(stderr, "name or value not found (create entry)\n");
        return error_return_400;
    }

    pthread_mutex_lock(&write_mutex);

    // get the index
    int index = get_hash_table_index(name);
    printf("INDEX FOR THE NAME: %d\n", index);

    HashTableEntry* entry = hash_table.hashTableValues[index];
    HashTableEntry* prev = NULL;

    // update the value if a name exists already
    while (entry && entry->name != NULL)
    {
        if(strcmp(entry->name, name) == 0){
            entry->value = value;
            pthread_mutex_unlock(&write_mutex);
            return name;
        }

        prev = entry;
        entry = entry->next;
    }
    

    HashTableEntry *new_entry = malloc(sizeof(HashTableEntry));;
    if (!new_entry){
        fprintf(stderr, "Failed to malloc for new entry (create entry)");
        pthread_mutex_unlock(&write_mutex);
        return error_return_400;
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
        hash_table.hashTableValues[index] = new_entry;
    }
    
    hash_table.size += 1;
    pthread_mutex_unlock(&write_mutex);

    printf("Entered the name and the value into the hash table!\n");
    return name;
}

char* read_entry(char *name){
    printf("Hash table size when trying to read: %d\n", hash_table.size);

    if(hash_table.size == 0){
        fprintf(stderr, "hash table is empty (read entry)\n");
        return NULL;
    }

    if(!name){
        fprintf(stderr, "name not properly received (read entry)\n");
        return NULL;
    }

    pthread_mutex_lock(&read_count_mutex);
    readers_count++;
    if(readers_count == 1) pthread_mutex_lock(&write_mutex);
    pthread_mutex_unlock(&read_count_mutex);

    int index = get_hash_table_index(name);
    HashTableEntry *entry = hash_table.hashTableValues[index];

    while(entry && entry->name != NULL){
        if(strcmp(entry->name, name) == 0){
            pthread_mutex_lock(&read_count_mutex);
            readers_count--;
            if(readers_count == 0) pthread_mutex_unlock(&write_mutex);
            pthread_mutex_unlock(&read_count_mutex);

            return entry->value;
        }

        entry = entry->next; 
    }

    fprintf(stderr, "name doesnt exist in the hash table (read entry)\n");

    pthread_mutex_lock(&read_count_mutex);
    readers_count--;
    if(readers_count == 0) pthread_mutex_unlock(&write_mutex);
    pthread_mutex_unlock(&read_count_mutex);

    return NULL;
}

char* delete_entry(char *name){
    char *error_return_400 = "400";
    char *empty = "empty";

    if(hash_table.size == 0){
        fprintf(stderr, "hash table is empty (delete entry)\n");
        return empty;
    }

    if(!name){
        fprintf(stderr, "name not properly received (delete entry)\n");
        return error_return_400;
    }

    pthread_mutex_lock(&write_mutex);

    int index = get_hash_table_index(name);

    HashTableEntry *entry = hash_table.hashTableValues[index];
    HashTableEntry *prev = NULL;

    while(entry && entry->name != NULL){
        if(strcmp(entry->name, name) == 0){
            if(entry->indexInList == 0){
                hash_table.hashTableValues[index] = entry->next;
            } else {
                prev->next = entry->next;
            }

            free(entry);
            hash_table.size -= 1;

            pthread_mutex_unlock(&write_mutex);
            
            printf("Entry found and deleted\n");
            return name;
        }

        entry = entry->next;
    }

    pthread_mutex_unlock(&write_mutex);

    fprintf(stderr, "Entry doesn't exist in the table for name: %s\n", name);
    return error_return_400;
}


// testing the functions

/* int main(int argc, char const *argv[])
{
    printf("\nINITIALISED HASH TABLE\n:");
    printf("HASH TABLE CAPACITY: %d\n", hash_table.capacity);

    char *name = "regional manager";
    char *name2 = "assistant to the regional manager";
    char *value = "Micheal Scott";
    char *value2 = "Dwight Schrute";

    create_entry(name , value);
    create_entry(name2 , value2);
    int index = get_hash_table_index(name);
    int index2 = get_hash_table_index(name2);

    printf("\nCHECKING IN MAIN IF CREATED: \n");
    printf("TABLE INDEX: %d\n", index);
    printf("TABLE INDEX FOR 2: %d\n", index2);
    printf("TABLE NAME FOR INDEX: %s\n", hash_table.hashTableValues[index]->name);
    printf("TABLE VALUE FOR INDEX: %s\n", hash_table.hashTableValues[index]->value);
    printf("HASH TABLE SIZE: %d\n", hash_table.size);

    printf("\nCHECKING IN MAIN IF READ: \n");
    printf("VALUE FOR THE NAME: %s\n", read_entry(name));
    printf("VALUE FOR THE NAME 2: %s\n", read_entry(name2));
    printf("HASH TABLE SIZE: %d\n", hash_table.size);

    printf("\nCHECKING IN MAIN IF DELETED: \n");
    if(!delete_entry(name)){
        printf("Entry deleted\n");
    } else {
        printf("something went wrong trying to delete your entry\n");
    }
    printf("VALUE FOR THE NAME: %s\n", read_entry(name));
    printf("VALUE FOR THE NAME 2: %s\n", read_entry(name2));
    printf("HASH TABLE SIZE: %d\n", hash_table.size);

    return 0;
} */
