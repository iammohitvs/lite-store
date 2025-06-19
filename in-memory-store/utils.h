#ifndef UTILS_H
#define UTILS_H

#define MAX_HASH_TABLE_CAPACITY 1024

typedef struct hash_table_entry{
    char* name;
    char* value;
    int indexInList;
    struct hash_table_entry* next;
} HashTableEntry;

typedef struct {
    HashTableEntry hashTableValues[MAX_HASH_TABLE_CAPACITY];
    int size;
    int capacity;
} HashTable;

int get_hash_table_index(const char *name);

#endif