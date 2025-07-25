#ifndef HASH_TABLE_H
#define HASH_TABLE_H

char* create_entry(char *name, char* value);

char* read_entry(char *name);

char* delete_entry(char *name);

void save_data_to_disk(const char *filename);

void load_data_from_disk(const char *filename);

#endif