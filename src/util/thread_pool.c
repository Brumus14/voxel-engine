#include "thread_pool.h"

#include "../data_structures/queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../util/log.h"

void *thread_pool_thread_main(void *arg) {
    struct thread_pool *pool = (struct thread_pool *)arg;
    struct priority_queue *tasks = &pool->tasks;
    pthread_mutex_t *tasks_lock = &pool->tasks_lock;
    pthread_cond_t *task_available = &pool->task_available;

    // Add destroy flag
    while (true) {
        pthread_mutex_lock(tasks_lock);

        while (priority_queue_is_empty(tasks)) {
            pthread_cond_wait(task_available, tasks_lock);
        }

        struct thread_pool_task *task = priority_queue_pop(tasks);
        pthread_mutex_unlock(tasks_lock);

        // Execute the task
        task->function(task->argument);
        free(task->argument);
        free(task);
    }

    return NULL;
}

void thread_pool_init(struct thread_pool *pool, unsigned int count) {
    pool->thread_count = count;
    priority_queue_init(&pool->tasks);
    pthread_mutex_init(&pool->tasks_lock, NULL);
    pthread_cond_init(&pool->task_available, NULL);

    pool->threads = malloc(sizeof(pthread_t) * count);

    for (unsigned int i = 0; i < count; i++) {
        pthread_create(&pool->threads[i], NULL, thread_pool_thread_main, pool);
    }
}

void thread_pool_destroy(struct thread_pool *pool) {
    for (unsigned int i = 0; i < pool->thread_count; i++) {
        pthread_cancel(pool->threads[i]);
    }

    free(pool->threads);

    pthread_cond_destroy(&pool->task_available);
    pthread_mutex_destroy(&pool->tasks_lock);

    struct thread_pool_task *task = priority_queue_pop(&pool->tasks);

    while (task) {
        free(task->argument);
        free(task);
        task = priority_queue_pop(&pool->tasks);
    }

    priority_queue_destroy(&pool->tasks);
}

void thread_pool_schedule(struct thread_pool *pool,
                          thread_pool_task_function function, void *argument,
                          float priority) {
    pthread_mutex_lock(&pool->tasks_lock);

    struct thread_pool_task *task = malloc(sizeof(struct thread_pool_task));
    task->function = function;
    task->argument = argument;

    priority_queue_push(&pool->tasks, task, priority);
    pthread_mutex_unlock(&pool->tasks_lock);

    pthread_cond_signal(&pool->task_available);
}
