#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "../data_structures/priority_queue.h"
#include "../threads/threads.h"
#include <unistd.h>

typedef void *(*thread_pool_task_function)(void *);

struct thread_pool_task {
    thread_pool_task_function function;
    void *argument; // Heap allocated, assumes argument is all on stack
};

struct thread_pool {
    unsigned int thread_count;
    struct thread *threads;
    struct priority_queue tasks;
    struct mutex tasks_lock;
    struct condition task_available;
};

void *thread_pool_task_main(void *arg);
void thread_pool_init(struct thread_pool *pool, unsigned int count);
void thread_pool_destroy(struct thread_pool *pool);
// Low is higher priority
void thread_pool_schedule(struct thread_pool *pool,
                          thread_pool_task_function function, void *argument,
                          float priority);

#endif
