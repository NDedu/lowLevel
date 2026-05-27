#ifndef KV_H
#define KV_H

#include <stdlib.h>
#include <sys/types.h>

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
ssize_t kv_put(kv_t *db, char *key, char *value); // returns the index, -1 on error, -2 if full
char *kv_get(kv_t *db, char *key); // returns the pointer to the value, NULL if not found
ssize_t kv_delete(kv_t *db, char *key); // returns the index, -1 on not found
int kv_free(kv_t *table); // returns 0 on success or -1 on failure, user must NULL db on free

#endif
