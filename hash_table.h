/**
 * @file hash_table.h
 *
 * @brief
 *
 *
 */
#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#define DESC_HASH_CAPACITY    1024
typedef struct
{
    char* key;
    void *value;
} hash_item_t;

struct overflow_list
{
    hash_item_t *item;
    struct overflow_list *next;
};

typedef struct overflow_list overflow_list_t;
typedef struct
{
    hash_item_t **items;
    overflow_list_t **overflow_buckets;
    int size;
    int count;
} hash_table_t;


hash_item_t  *create_hash_item(char* key, void *value);
hash_table_t* create_hash_table(int size);
void free_hash_item(hash_item_t* item);
void free_hash_table(hash_table_t *table);
void print_hash_table(hash_table_t *table);
unsigned long hash(char *str);
void hash_table_insert_key(hash_table_t *table, char *key, void *value);
data_desc_t *hash_table_key_search(hash_table_t *table, char *key);

#endif //__HASH_TABLE_H__
