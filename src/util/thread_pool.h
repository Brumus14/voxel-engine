#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../data_structures/queue.h"
#include <pthread.h>
#include <unistd.h>

typedef void *(*thread_pool_task_function)(void *);

struct thread_pool_task {
    thread_pool_task_function function;
    void *argument;
};

struct thread_pool {
    unsigned int thread_count;
    pthread_t *threads;
    struct queue tasks;
    pthread_mutex_t tasks_lock;
    pthread_cond_t task_available;
};

void *thread_pool_task_main(void *arg);
void thread_pool_init(struct thread_pool *pool, unsigned int count);
void thread_pool_schedule(struct thread_pool *pool,
                          thread_pool_task_function function, void *argument);

#endif
