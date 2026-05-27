#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "kv.h"

int main() {

    kv_t *table = kv_init(3);
    printf("%p\n", table);
    kv_free(table);
    table = NULL;

    kv_t *db = kv_init(16);
    assert(db != NULL);
    assert(db->capacity == 16);
    assert(db->count == 0);
    assert(kv_put(db, "key1", "value1") >= 0);
    ssize_t k2 = kv_put(db, "key2", "value2");
    assert(k2 >= 0);
    assert(kv_put(db, "key2", "value2.1") == k2); // update returns same slot
    assert(kv_put(db, "key3", "value3") >= 0);
    assert(db->count == 3); // key2 updated, not re-inserted
    assert(strcmp(kv_get(db, "key2"), "value2.1") == 0);
    assert(kv_put(db, NULL, "x") == -1);
    assert(kv_put(db, "k", NULL) == -1);

    for (int i = 0; i < db->capacity; i++) {
        if (db->entries[i].key) {
            printf("[%d] %s: %s\n", i, db->entries[i].key, db->entries[i].value);
        }
    }
    kv_free(db);
    db = NULL;

    kv_t *db2 = kv_init(16);
    kv_put(db2, "name", "alice");
    assert(strcmp(kv_get(db2, "name"), "alice") == 0);
    assert(kv_get(db2, "missing") == NULL);
    kv_free(db2);
    db2 = NULL;

    kv_t *db3 = kv_init(16);
    kv_put(db3, "name", "alice");
    kv_put(db3, "city", "berlin");
    assert(kv_delete(db3, "name") >= 0);
    assert(kv_get(db3, "name") == NULL);
    assert(db3->count == 1);
    assert(kv_delete(db3, "missing") == -1);
    kv_free(db3);
    db3 = NULL;

    return 0;
}
