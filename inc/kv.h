#ifndef KV_H
#define KV_H

#include <stdlib.h>

typedef struct {
    char    *key;
    char    *value;
} kv_entry_t;

typedef struct {
    size_t      capacity;
    size_t      count;
    kv_entry_t  *entries;
} kv_t;

kv_t *kv_init(size_t capacity); // returns a kv_t pointer
int kv_put(kv_t *db, char *key, char *value); // returns the index, -1 on error, -2 if full
char *kv_get(kv_t *db, char *key);
void kv_free(kv_t *table);

#endif
