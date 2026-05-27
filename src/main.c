#include <stdio.h>
#include <assert.h>
#include "kv.h"

int main() {

    kv_t *table = kv_init(3);
    printf("%p\n", table);
    kv_free(table);

    kv_t *db = kv_init(16);
    assert(db != NULL);
    assert(db->capacity == 16);
    assert(db->count == 0);

    kv_put(db, "key1", "value1");
    kv_put(db, "key2", "value2");
    kv_put(db, "key2", "value2.1");
    kv_put(db, "key3", "value3");

    for (int i = 0; i < db->capacity; i++) {
        if (db->entries[i].key) {
            printf("[%d] %s: %s\n", i, db->entries[i].key, db->entries[i].value);
        }
    }

    kv_free(db);

    return 0;
}
