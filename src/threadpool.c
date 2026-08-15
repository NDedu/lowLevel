#include "threadpool.h"
#include <pthread.h>
#include <stddef.h>

static void *worker(void *threadpool) {
    threadpool_t *pool = (threadpool_t *)threadpool;

    while (1) {
        pthread_mutex_lock(&(pool->lock));

        while (pool->queued == 0 && !pool->stop) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if (pool->stop && pool->queued == 0) {
            pthread_mutex_unlock(&(pool->lock));
            break;
        }

        task_t task = pool->task_queue[pool->queue_front];
        pool->queue_front = (pool->queue_front + 1) % QUEUE_SIZE;
        pool->queued--;

        pthread_mutex_unlock(&(pool->lock));

        task.function(task.arg);
    }

    return NULL;
}

int threadpool_init(threadpool_t *pool) {
    if (!pool) return -1;

    pool->started = 0;
    pool->queued = 0;
    pool->queue_front = 0;
    pool->queue_back = 0;
    pool->stop = 0;

    if (pthread_mutex_init(&(pool->lock), NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&(pool->notify), NULL) != 0) {
        pthread_mutex_destroy(&(pool->lock));
        return -1;
    }

    for (int i = 0; i < THREADS; i++) {
        if (pthread_create(&(pool->threads[i]), NULL, worker, pool) != 0) {
            threadpool_destroy(pool);
            return -1;
        }
        pool->started++;
    }

    return 0;
}

int threadpool_add_task(threadpool_t *pool, void (*function)(void *arg), void *arg) {
    if (!pool || !function) return -1;

    if (pthread_mutex_lock(&(pool->lock)) != 0) return -1;

    if (pool->stop) {
        pthread_mutex_unlock(&(pool->lock));
        return -1;
    }

    if (pool->queued == QUEUE_SIZE) {
        pthread_mutex_unlock(&(pool->lock));
        return -2;
    }

    pool->task_queue[pool->queue_back].function = function;
    pool->task_queue[pool->queue_back].arg = arg;
    pool->queue_back = (pool->queue_back + 1) % QUEUE_SIZE;
    pool->queued++;

    pthread_cond_signal(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    return 0;
}

int threadpool_destroy(threadpool_t *pool) {
    if (!pool) return -1;

    if (pthread_mutex_lock(&(pool->lock)) != 0) return -1;
    pool->stop = 1;
    pthread_cond_broadcast(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));

    for (int i = 0; i < pool->started; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));

    return 0;
}
