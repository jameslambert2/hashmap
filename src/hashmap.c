#include <exit_codes.h>
#include <hashmap.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Values needed for FNV-1 Hash
#define FNV_PRIME 0x100000001b3
#define FNV_OFFSET 0xcbf29ce484222325

typedef struct data_point data_point_t;
struct data_point
{
    const char *key;
    void *value;
    data_point_t *overflow;
    uint64_t key_hash;
};

typedef struct hash_table
{
    data_point_t **table;
    uint64_t max_size;
    uint64_t spots_used;
    uint64_t items_in_table;
    void (*free_func)(void *);
    void (*print_func)(void *);
} hash_table_t;

/**
  EXAMPLE PSUEDOCODE FOR HASH ALGORITHM
    hash := FNV_OFFSET

    for each byte_of_data to be hashed do
        hash := hash × FNV_PRIME
        hash := hash XOR byte_of_data

    return hash
*/
// Modified from code found at below location
// SOURCE: https://benhoyt.com/writings/hash-table-in-c/
static uint64_t FNVHash(const char *key, uint64_t length)
{
    uint64_t hash = FNV_OFFSET;
    for (uint64_t i = 0; i < length; i++)
    {
        hash ^= key[i];
        hash *= FNV_PRIME;
    }
    return hash;
}

static uint64_t get_index(const char *key, const hash_table_t *h_table)
{
    if ((NULL == h_table) || (NULL == key))
    {
        return UINT64_MAX;
    }
    uint64_t size = strlen(key); // Check String is NULL Terminated
    uint64_t hash = FNVHash(key, size);
    return (hash % h_table->max_size);
}

data_point_t *make_data_point(const char *key, void *value)
{
    data_point_t *ret_data_point =
        (data_point_t *)calloc(1, sizeof(*ret_data_point));
    if (NULL == ret_data_point)
    {
        return NULL;
    }
    ret_data_point->key = key;
    ret_data_point->value = value;
    return ret_data_point;
}

exit_code_t hash_insert_hashed_val(hash_table_t *h_table, uint64_t hash,
                                   void *value)
{
    if ((NULL == h_table) || (NULL == value))
    {
        return E_NULL_PARAMS;
    }
    uint64_t idx = hash % h_table->max_size;
    data_point_t *data = calloc(1, sizeof(data_point_t));
    if (NULL == data)
    {
        return E_ALLOC_ERROR;
    }
    data->key = NULL;
    data->value = value;
    data->overflow = NULL;
    data->key_hash = hash;
    if (NULL == h_table->table[idx])
    {
        h_table->table[idx] = data;
        h_table->spots_used++;
    }
    else
    {
        data_point_t *temp = h_table->table[idx];
        while (NULL != temp->overflow)
        {
            temp = temp->overflow;
        }
        temp->overflow = data;
    }
    h_table->items_in_table++;
    return E_SUCCESS;
}

void *hash_search_hashed_val(hash_table_t *h_table, uint64_t hash)
{
    void *ret_val = NULL;
    data_point_t *temp = NULL;
    if ((NULL == h_table))
    {
        goto END;
    }
    uint64_t index = hash % h_table->max_size;
    temp = h_table->table[index];
    while (NULL != temp)
    {
        if (hash != temp->key_hash)
        {
            temp = temp->overflow;
        }
        else
        {
            ret_val = temp->value;
            break;
        }
    }
END:
    return ret_val;
}

exit_code_t hash_insert(hash_table_t *h_table, const char *key, void *value)
{
    if ((NULL == h_table) || (NULL == value) || (NULL == key))
    {
        return E_NULL_PARAMS;
    }
    uint32_t idx = get_index(key, h_table);
    data_point_t *data = make_data_point(key, value);
    if (NULL == data)
    {
        return E_NULL_PARAMS;
    }
    if (NULL == h_table->table[idx])
    {
        h_table->table[idx] = data;
        h_table->spots_used++;
    }
    else if (0 == strcmp(key, h_table->table[idx]->key))
    {
        h_table->free_func(data->value);
        free(data);
        return E_KEY_ALREADY_EXISTS;
    }
    else
    {
        data_point_t *temp = h_table->table[idx];
        while (NULL != temp->overflow)
        {
            if (0 == strcmp(key, temp->key))
            {
                return E_KEY_ALREADY_EXISTS;
            }
            temp = temp->overflow;
        }
        temp->overflow = data;
    }
    h_table->items_in_table++;
    return E_SUCCESS;
}

void *hash_search(const hash_table_t *h_table, const char *key)
{
    if ((NULL == h_table) || (NULL == key))
    {
        return NULL;
    }
    uint64_t index = get_index(key, h_table);
    if (h_table->max_size < index)
    {
        return NULL;
    }
    if (NULL == h_table->table[index])
    {
        return NULL;
    }
    data_point_t *temp = h_table->table[index];
    while (NULL != temp)
    {
        if (0 == strcmp(temp->key, key))
        {
            break;
        }
        temp = temp->overflow;
    }
    if (NULL == temp)
    {
        return NULL;
    }
    return temp->value;
}

exit_code_t hash_remove(hash_table_t *h_table, const char *key)
{
    if ((NULL == h_table) || (NULL == key))
    {
        return E_NULL_PARAMS;
    }
    uint64_t index = get_index(key, h_table);
    if (NULL != h_table->table[index])
    {
        data_point_t *temp = h_table->table[index]->overflow;
        if (0 == strcmp(key, h_table->table[index]->key))
        {
            if (NULL != h_table->free_func)
            {
                h_table->free_func(h_table->table[index]->value);
            }
            free(h_table->table[index]);
            h_table->table[index] = temp;
            return E_SUCCESS;
        }
        else
        {
            data_point_t *prior = h_table->table[index];
            while (NULL != temp)
            {
                if (0 == strcmp(key, temp->key))
                {
                    prior->overflow = temp->overflow;
                    if (NULL != h_table->free_func)
                    {
                        h_table->free_func(temp->value);
                    }
                    free(temp);
                    return E_SUCCESS;
                }
                prior = temp;
                temp = temp->overflow;
            }
            return E_KEY_NOT_FOUND;
        }
    }
    return E_KEY_NOT_FOUND;
}

void display(const hash_table_t *h_table)
{
    if (NULL == h_table->print_func)
    {
        fprintf(stderr, "No print function provided\n");
        return;
    }
    uint64_t i = 0;
    printf("[");
    for (i = 0; i < h_table->max_size; i++)
    {
        if (NULL != h_table->table[i])
        {
            printf("{%s : ", h_table->table[i]->key);
            h_table->print_func(h_table->table[i]->value);
            printf("}");
            data_point_t *temp = h_table->table[i]->overflow;
            while (NULL != temp)
            {
                printf("-->{%s : ", temp->key);
                h_table->print_func(temp->value);
                printf("}");
                temp = temp->overflow;
            }
        }
        else
            printf(" ~~ ");
        if (h_table->max_size - 1 != i)
            printf(", ");
    }

    printf("]\n");
}

hash_table_t *hash_table_init(uint64_t size, void (*free_func)(void *),
                              void (*print_func)(void *))
{
    if (0 == size)
    {
        return NULL;
    }
    hash_table_t *h_table = (hash_table_t *)calloc(1, sizeof(*h_table));
    if (NULL == h_table)
    {
        return NULL;
    }
    h_table->table = (data_point_t **)calloc(size, sizeof(data_point_t *));
    if (NULL == h_table->table)
    {
        free(h_table);
        return NULL;
    }
    h_table->free_func = free_func;
    h_table->print_func = print_func;
    h_table->max_size = size;
    return h_table;
}

static void clear_overflow(data_point_t *data, void (*free_func)(void *))
{
    if (NULL == data)
    {
        return;
    }
    data_point_t *next = data->overflow;
    if (NULL != free_func)
    {
        free_func(data->value);
    }
    free(data);
    clear_overflow(next, free_func);
}

exit_code_t hash_table_destroy(hash_table_t *h_table)
{
    if (NULL == h_table)
    {
        return E_NULL_PARAMS;
    }
    for (int idx = 0; idx < h_table->max_size; idx++)
    {
        clear_overflow(h_table->table[idx], h_table->free_func);
    }
    free(h_table->table);
    free(h_table);
    return E_SUCCESS;
}
