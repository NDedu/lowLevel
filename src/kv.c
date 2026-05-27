#include "kv.h"
#include <stdlib.h>
#include <string.h>

#define TOMBSTONE 0x1

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

// fn kv_put
// params:
//  - db: pointer to the db
//  - key: pointer to the key value
//  - value: pointer to the value itself
// returns the index of the key , otherwise on error returns -1; on not found -2

size_t hash(char *val, size_t capacity) {
    size_t hash = 0x13371337deadbeef;

    while (*val) {
        hash ^= *val;
        hash = hash << 8;
        hash += *val;
        val++;
    }
    return hash % capacity;
}

int kv_put(kv_t *db, char *key, char *value) {
    if (!db || !key || !value) return -1;

    size_t index = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity; i++) {
        size_t real_index = (index + i) % db->capacity;
        kv_entry_t *entry = &db->entries[real_index];

        // key set, just update
        if (entry->key && entry->key != (void*)TOMBSTONE && !strcmp(entry->key, key)) {
            char *newval = strdup(value);

            if (!newval) return -1;
            free(entry->value); // free the previous value before overwriting
            entry->value = newval;
            return 0;
        }

        // land in "empty" slot, null/tombstone (deleted)
        if (!entry->key || entry->key == (void*)TOMBSTONE) {
            char *newval = strdup(value);
            char *newkey = strdup(key);

            if (!newval || !newkey) {
                free(newval);
                free(newkey); // free both in case one allocation succeds and the other does not
                return -1;
            }
            entry->value = newval;
            entry->key = newkey;
            db->count++;
            return 0;
        }
    }

    return -2;
}

// fn kv_get
// params:
//  - db: pointer to the db
//  - key: pointer to the key value
// returns the pointer to the value, or NULL if not found

char *kv_get(kv_t *db, char *key) {
    if (!db || !key) return NULL;

    size_t index = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity; i++) {
        size_t real_index = (index + i) % db->capacity;

        kv_entry_t *entry = &db->entries[real_index];

        if (entry->key == NULL) return NULL;

        if (entry->key && entry->key != (void*)TOMBSTONE && !strcmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

// fn kv_delete
// params:
//  - db: pointer to the db
//  - key: pointer to the key value
// returns the index of the deleted item, or -1 on not found

int kv_delete(kv_t *db, char *key) {
    if (!db || !key) return -1;

    size_t index = hash(key, db->capacity);

    for (size_t i = 0; i < db->capacity; i++) {
        size_t real_index = (index + i) % db->capacity;
        kv_entry_t *entry = &db->entries[real_index];

        if (entry->key == NULL) return -1;

        if (entry->key && entry->key != (void*)TOMBSTONE && !strcmp(entry->key, key)) {
            free(entry->key);
            free(entry->value);
            db->count--;
            entry->key = (void*)TOMBSTONE;
            entry->value = NULL;

            return real_index;
        }
    }

    return -1;
}

// fn kv_free
// params:
//  - *kv_table: pointer to the kv table
// returns 0 on success or -1 on failure
int kv_free(kv_t *db) {
    if (!db) return -1;

    for (size_t i = 0; i < db->capacity; i++) {
        kv_entry_t *entry = &db->entries[i];

        if (entry->key && entry->key != (void*)TOMBSTONE) {
            free(entry->key);
            free(entry->value);
            entry->key = NULL;
            entry->value = NULL;
            db->count--;
        }
    }

    free(db->entries);
    free(db);
    return 0;
}
