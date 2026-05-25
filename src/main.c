#include <stdio.h>
#include <assert.h>
#include "kv.h"

int main() {

    kv_t *table = kv_init(3);
    printf("%p\n", table);

    kv_t *db = kv_init(16);
    assert(db != NULL);
    assert(db->capacity == 16);
    assert(db->count == 0);
    kv_free(db);

    return 0;
}
