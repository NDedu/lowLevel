#include "threadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void example_task(void* arg) {
    int* num = (int*)arg;
    printf("Processing task %d\n", *num);
    sleep(1);
    free(num);
}

int main() {
    threadpool_t pool;
    if (threadpool_init(&pool) != 0) {
        fprintf(stderr, "threadpool_init failed\n");
        return 1;
    }

    for (int i = 0; i < 15; i++) {
        int* task_num = malloc(sizeof(int));
        if (!task_num) break;
        *task_num = i;

        if (threadpool_add_task(&pool, example_task, task_num) != 0) {
            free(task_num);
        }
    }

    threadpool_destroy(&pool);

    return 0;
}
