#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include "queue.h"
#include "stdbool.h"
#include <pthread.h>

struct safe_queue {
    struct queue queue;
    pthread_mutex_t lock;
};

void safe_queue_init(struct safe_queue *queue, unsigned long data_size);
void safe_queue_destroy(struct safe_queue *queue);
void safe_queue_enqueue(struct safe_queue *queue, void *data);
void *safe_queue_dequeue(struct safe_queue *queue);
bool safe_queue_is_empty(struct safe_queue *queue);

#endif
