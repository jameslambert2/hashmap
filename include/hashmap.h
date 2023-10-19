#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct hash_table hash_table_t;

typedef enum hash_exit
{
    /* SUCCESS */
    E_SUCCESS,

    /* KEY ERRORS*/
    E_KEY_ALREADY_EXISTS,
    E_KEY_NOT_FOUND,

    /* NULL ERRORS */
    E_NULL_PARAMS,
    E_ALLOC_ERROR
} exit_code_t;

/**
 * @brief Initialization function for hash_table_t
 *
 * @param size max size for the hash_table
 * @param free_func method of freeing items in the hash_table (if no free
 * necessary, use NULL)
 * @param print_func method of printing items in the hash_table (if no print
 * needed, use NULL)
 * @return hash_table_t* object pointer to hash_table_t
 */
hash_table_t *hash_table_init(uint64_t size, void (*free_func)(void *),
                              void (*print_func)(void *));

/**
 * @brief Destroy function for hash_table_t object
 *
 * @param h_table hash_table_t object pointer to be destroyed
 * @return exit_code_t - E_SUCCESS on clean destroy, E_NULL_POINTER if pointer
 * object passed is NULL
 */
exit_code_t hash_table_destroy(hash_table_t *h_table);

/**
 * @brief Function to insert objects into the hash_table_t object
 *
 * @param h_table hash_table_t object to house the key value pair
 * @param key a NULL-terminated const char* to be used for lookup
 * @param value any pointer containing data to be added to the hash_table_t
 * @return exit_code_t E_SUCCESS on successful insertion, E_NULL_POINTER if
 * h_table, key, or value is NULL
 */
exit_code_t hash_insert(hash_table_t *h_table, const char *key, void *value);

/**
 * @brief Internal function to allow for printing whats is currently in the hash
 * table
 *
 * @param h_table hash_table_t object pointer that you would like displayed
 */
void display(const hash_table_t *h_table);

/**
 * @brief a lookup method of getting the value based on a key
 *
 * @param h_table hash_table_t to lookup information in
 * @param key a NULL-terminated const char* used for lookup
 * @return void* value associated with the key
 */
void *hash_search(hash_table_t *h_table, const char *key);

/**
 * @brief removes a key value pair from the hash_table_t
 *
 * @param h_table hash_table_t object to remove the key value pair from
 * @param key a NULL-terminated const char* used for lookup and removal
 * @return exit_code_t E_SUCCESS if item was successfully removed,
 * E_KEY_NOT_FOUND if key is not in hash_table_t
 */
exit_code_t hash_remove(hash_table_t *h_table, const char *key);

/**
 * @brief a hash insertion function that relies on the hashed value instead
 * of the unhashed char*
 *
 * @param h_table hash_table_t * the hash table for the information to be
 * inserted into
 * @param hash hashed value to be inserted into the hash table
 * @param value the value associated with the provided hashed value
 * @return exit_code_t E_SUCCESS on successful insertion, E_NULL_PARAMS
 * if a NULL parameter was passed, and E_ALLOC_FAILURE if memory could not be
 * allocated
 */
exit_code_t hash_insert_hashed_val(hash_table_t *h_table, uint64_t hash,
                                   void *value);

/**
 * @brief
 *
 * @param h_table hash_table_t * the hash table for the information to be
 * inserted into
 * @param hash hashed value that you are searching for
 * @return void* value associated with the hashed value. NULL is returned
 * if some error occurs, or the hashed value is not found
 */
void *hash_search_hashed_val(hash_table_t *h_table, uint64_t hash);

#endif /* HASH_TABLE_H */