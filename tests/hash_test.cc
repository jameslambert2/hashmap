#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
extern "C"
{
#include "hashmap.h"
}

pthread_t tid[20]; 
int counter; 
pthread_mutex_t lock; 
struct temp {
    const char *key;
    hash_table_t *table;
};

void *thread_func(void *h_table)
{
    if (NULL == h_table)
    {
        return NULL;
    }
    hash_table_t *temp_h_table = ((struct temp*)h_table)->table;
    const char *temp = "Temp";
    const char * temp_key = ((struct temp*)h_table)->key;
    hash_insert(temp_h_table, temp_key, (void*)temp);
    free(h_table);
    return NULL;
}

void *thread_func2(void *h_table)
{
    if (NULL == h_table)
    {
        return NULL;
    }
    hash_table_t *temp_h_table = ((struct temp*)h_table)->table;
    const char * temp_key = ((struct temp*)h_table)->key;
    exit_code_t temp = hash_remove(temp_h_table, temp_key);
    if (temp != E_SUCCESS)
    {
        printf("Failure\n");
    }
    free(h_table);
    return NULL;
}

TEST(BaseTest, thread_safe_testing)
{ 
    int i = 0; 
    hash_table_t *h_table = hash_table_init(1,NULL,NULL);
    
    const char *keys[10] = {"key 0","key 1","key 2","key 3", "key 4","key 5","key 6","key 7", "key 8", "key 9"};
    while (i < 10) 
    {
        struct temp *data = (struct temp*)calloc(1, sizeof(*data));
        data->table = h_table;
        data->key = keys[i];

        pthread_create(&(tid[i]), NULL, thread_func, data); 
        i++;
    }
    
    sleep(1);
    void* response = NULL;
    for (int i = 0; i < 10; i++)
    {
        response = hash_search(h_table, keys[i]);
        EXPECT_NE(response, nullptr);
    }

    while (i < 20) 
    {
        struct temp *data = (struct temp*)calloc(1, sizeof(*data));
        data->table = h_table;
        data->key = keys[i-10];

        pthread_create(&(tid[i]), NULL, thread_func2, data); 
        i++;
    } 
  
    for (int j = 0; j < 20; j++)
    {
        pthread_join(tid[j], NULL); 
    }
    pthread_mutex_destroy(&lock); 
  
    
    hash_table_destroy(h_table);
}