/**
 * @file hash_table.c
 *
 * @brief Simple hash table implemetationto to store individiual param description
 *
 *
 */


#include "hash_table.h"

overflow_list_t *allocate_overflow_list()
{
    // Allocates memory for a overflow_list_t pointer.
    overflow_list_t *list = (overflow_list_t *)malloc(sizeof(overflow_list_t));
    return list;
}
overflow_list_t *overflow_list_insert(overflow_list_t *list, hash_item_t *item)
{
    // Inserts the item onto the overflow_list_t.
    if (!list)
    {
        overflow_list_t *head = allocate_overflow_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    }
    else if (list->next == NULL)
    {
        overflow_list_t *node = allocate_overflow_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    overflow_list_t *temp = list;

    while (temp->next->next)
    {
        temp = temp->next;
    }

    overflow_list_t *node = allocate_overflow_list();
    node->item = item;
    node->next = NULL;
    temp->next = node;
    return list;
}
hash_item_t *overflow_list_remove(overflow_list_t *list)
{
    // Removes the head from the overflow_list_t.
    // Returns the item of the popped element.
    if (!list)
        return NULL;

    if (!list->next)
        return NULL;

    overflow_list_t *node = list->next;
    overflow_list_t *temp = list;
    temp->next = NULL;
    list = node;
    hash_item_t *it = NULL;
    memcpy(temp->item, it, sizeof(hash_item_t));
    free(temp->item->key);
    free(temp->item->value);
    free(temp->item);
    free(temp);
    return it;
}
void free_overflow_list(overflow_list_t *list)
{
    overflow_list_t *temp = list;
    while (list)
    {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}

overflow_list_t **create_overflow_buckets(hash_table_t *table)
{
    // Create the overflow buckets; an array of overflow_list.
    overflow_list_t **buckets = (overflow_list_t **)calloc(table->size, sizeof(overflow_list_t *));

    for (int i = 0; i < table->size; i++)
        buckets[i] = NULL;

    return buckets;
}

void free_overflow_buckets(hash_table_t *table)
{
    // Free all the overflow bucket lists.
    overflow_list_t **buckets = table->overflow_buckets;

    for (int i = 0; i < table->size; i++)
        free_overflow_list(buckets[i]);

    free(buckets);
}
hash_item_t  *create_hash_item(char* key, void *value)
{
    hash_item_t *item = (hash_item_t *) malloc(sizeof(hash_item_t));
    item->key = (char*) malloc(strlen(key) + 1);
    //item->value = (data_desc_t *) malloc(sizeof(value));
    item->value = value;
    strcpy(item->key, key);
    //memcpy(item->value, &value, sizeof(value));
    return item;
}
hash_table_t* create_hash_table(int size)
{
    hash_table_t* table = (hash_table_t*) malloc(sizeof(hash_table_t));
    table->size = size;
    table->count = 0;
    table->items = (hash_item_t**) calloc(table->size, sizeof(hash_item_t*));

    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;
    table->overflow_buckets = create_overflow_buckets(table);
    return table;
}
void free_hash_item(hash_item_t* item)
{
    free(item->key);
    free(item->value);
    free(item);
}

void free_hash_table(hash_table_t *table)
{
    for (int i = 0; i < table->size; i++)
    {
        hash_item_t *item = table->items[i];

        if (item != NULL)
            free_hash_item(item);
    }
    free_overflow_buckets(table);
    free(table->items);
    free(table);
}

void print_hash_table(hash_table_t *table)
{
    printf("\nHash Table\n-------------------\n");
    for (int i = 0; i < table->size; i++)
    {
        if (table->items[i])
        {
            printf("Index:%d, Key:%s, param ptr:%p\n", i, table->items[i]->key, table->items[i]->value);
        }
    }
    printf("-------------------\n\n");
}

unsigned long hash(char *str)
{
    unsigned long i = 0;

    for (int j = 0; str[j]; j++)
        i += str[j];

    return i % DESC_HASH_CAPACITY;
}
void handle_collision(hash_table_t *table, unsigned long index, hash_item_t *item)
{
    overflow_list_t *head = table->overflow_buckets[index];

    if (head == NULL)
    {
        // Creates the list.
        head = allocate_overflow_list();
        head->item = item;
        table->overflow_buckets[index] = head;
        return;
    }
    else
    {
        // Insert to the list.
        table->overflow_buckets[index] = overflow_list_insert(head, item);
        return;
    }
}
void hash_table_insert_key(hash_table_t *table, char *key, void *value)
{
    // Creates the item.
    hash_item_t *item = create_hash_item(key, value);
    // Computes the index.
    int index = hash(key);
    hash_item_t *current_item = table->items[index];
    if (current_item == NULL)
    {
        // Key does not exist.
        if (table->count == table->size)
        {
            // HashTable is full.
            printf("Insert Error: Hash Table is full\n");
            free_hash_item(item);
            return;
        }
        // Insert directly.
        table->items[index] = item;
        table->count++;
    }
    else
    {
        // Scenario 1: Update the value.
        if (strcmp(current_item->key, key) == 0)
        {
            //memcpy(table->items[index]->value, &value, sizeof(data_desc_t));
	    table->items[index]->value = value;
            return;
        }
        else
        {
            // Scenario 2: Handle the collision.
            handle_collision(table, index, item);
            return;
        }
    }
}
data_desc_t *hash_table_key_search(hash_table_t *table, char *key)
{
    int index = hash(key);
    if (table == NULL)
        return NULL;
    hash_item_t *item = table->items[index];
    overflow_list_t *head = table->overflow_buckets[index];
    if (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
            return item->value;
        if (head == NULL)
            return NULL;

        item = head->item;
        head = head->next;
    }
    return NULL;
}
void hash_table_delete_key(hash_table_t *table, char *key)
{
    // Deletes an item from the table.
    int index = hash(key);
    hash_item_t *item = table->items[index];
    overflow_list_t *head = table->overflow_buckets[index];
    if (item == NULL)
    {
        // Does not exist.
        return;
    }
    else
    {
        if (head == NULL && strcmp(item->key, key) == 0)
        {
            // Collision chain does not exist.
            // Remove the item.
            // Set table index to NULL.
            table->items[index] = NULL;
            free_hash_item(item);
            table->count--;
            return;
        }
        else if (head != NULL)
        {
            // Collision chain exists.
            if (strcmp(item->key, key) == 0)
            {
                // Remove this item.
                // Set the head of the list as the new item.
                free_hash_item(item);
                overflow_list_t *node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_hash_item(node->item->key, (node->item->value));
                free_overflow_list(node);
                table->overflow_buckets[index] = head;
                return;
            }

            overflow_list_t *curr = head;
            overflow_list_t *prev = NULL;

            while (curr)
            {
                if (strcmp(curr->item->key, key) == 0)
                {
                    if (prev == NULL)
                    {
                        // First element of the chain.
                        // Remove the chain.
                        free_overflow_list(head);
                        table->overflow_buckets[index] = NULL;
                        return;
                    }
                    else
                    {
                        // This is somewhere in the chain.
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_overflow_list(curr);
                        table->overflow_buckets[index] = head;
                        return;
                    }
                }

                curr = curr->next;
                prev = curr;
            }
        }
    }
}

