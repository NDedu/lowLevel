#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

#define THREADS 8
#define QUEUE_SIZE 100

typedef struct {
    void (*function)(void *arg);
    void *arg;
} task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  notify;
    pthread_t       threads[THREADS];
    task_t          task_queue[QUEUE_SIZE];
    unsigned        magic; // set while the pool is live, cleared by destroy
    int             started;
    int             queued;
    int             queue_front;
    int             queue_back;
    int             stop;
} threadpool_t;

// A pool must not be destroyed concurrently with add_task: once destroy starts
// joining, the caller owns the pool again. Calls that follow a destroy are
// rejected rather than touching the freed synchronisation primitives.
int threadpool_init(threadpool_t *pool); // returns 0 on success, -1 on failure
int threadpool_add_task(threadpool_t *pool, void (*function)(void *arg), void *arg); // returns 0 on success, -1 on error or once the pool is stopped, -2 if the queue is full
int threadpool_destroy(threadpool_t *pool); // cleans the queue then joins every worker; returns 0 on success, -1 on failure or if the pool is not live

#endif
