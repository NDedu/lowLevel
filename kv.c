#include "kv.h"

kv_t *kv_init(size_t capacity) {
    if (capacity == 0) return NULL;

    kv_t *table = malloc(sizeof(kv_t));
    if (table == NULL) {
        return NULL;
    }

    table->capacity = capacity;
    table->count = 0;

    table->entries = calloc(capacity, sizeof(kv_entry_t)); // allocates a large block of memory as a function of types and number of types; allocates capacity number of kv_entry_t
    if (table->entries == NULL) {
        free(table);
        return NULL;
    }

    return table;
}

void kv_free(kv_t *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < table->capacity; i++) {
        free(table->entries[i].kv);
        free(table->entries[i].value);
    }

    free(table->entries);
    free(table);
}
