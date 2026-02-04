#include "safe_queue.h"
#include "queue.h"
#include <pthread.h>

void safe_queue_init(struct safe_queue *queue, unsigned long data_size) {
    queue_init(&queue->queue, data_size);
    pthread_mutex_init(&queue->lock, NULL);
}

void safe_queue_destroy(struct safe_queue *queue) {
    pthread_mutex_destroy(&queue->lock);
    queue_destroy(&queue->queue);
}

void safe_queue_enqueue(struct safe_queue *queue, void *data) {
    pthread_mutex_lock(&queue->lock);
    queue_enqueue(&queue->queue, data);
    pthread_mutex_unlock(&queue->lock);
}

void *safe_queue_dequeue(struct safe_queue *queue) {
    pthread_mutex_lock(&queue->lock);
    void *item = queue_dequeue(&queue->queue);
    pthread_mutex_unlock(&queue->lock);

    return item;
}

bool safe_queue_is_empty(struct safe_queue *queue) {
    pthread_mutex_lock(&queue->lock);
    bool empty = queue_is_empty(&queue->queue);
    pthread_mutex_unlock(&queue->lock);

    return empty;
}

int safe_queue_length(struct safe_queue *queue) {
    pthread_mutex_lock(&queue->lock);
    int length = queue_length(&queue->queue);
    pthread_mutex_unlock(&queue->lock);

    return length;
}

void *safe_queue_get(struct safe_queue *queue, int index) {
    pthread_mutex_lock(&queue->lock);
    void *item = queue_get(&queue->queue, index);
    pthread_mutex_unlock(&queue->lock);

    return item;
}

int safe_queue_find(struct safe_queue *queue, void *data) {
    pthread_mutex_lock(&queue->lock);
    int index = queue_find(&queue->queue, data);
    pthread_mutex_unlock(&queue->lock);

    return index;
}
