#include "spmc_queue.h"

#include "stdlib.h"
#include <stddef.h>

void spmc_queue_node_init(struct spmc_queue_node *node, void *data) {
    node->data = data;
    node->next = NULL;
}

void spmc_queue_init(struct spmc_queue *queue) {
    struct spmc_queue_node *sentinel = malloc(sizeof(struct spmc_queue_node));
    queue->head = sentinel;
    queue->tail = sentinel;
    pthread_mutex_init(&queue->head_mutex, NULL);
}

void spmc_queue_destroy(struct spmc_queue *queue) {
    while (!spmc_queue_is_empty(queue)) {
        spmc_queue_dequeue(queue);
    }

    // Destroy sentinel
    free(queue->head);

    pthread_mutex_destroy(&queue->head_mutex);
}

void spmc_queue_enqueue(struct spmc_queue *queue, void *data) {
    struct spmc_queue_node *new_node = malloc(sizeof(struct spmc_queue_node));
    spmc_queue_node_init(new_node, data);

    queue->tail->next = new_node;
}

void *spmc_queue_dequeue(struct spmc_queue *queue) {
    pthread_mutex_lock(&queue->head_mutex);
    struct spmc_queue_node *node = queue->head->next;

    if (!node) {
        pthread_mutex_unlock(&queue->head_mutex);
        return NULL;
    }

    queue->head->next = node->next;

    void *data = node->data;
    free(node);

    pthread_mutex_unlock(&queue->head_mutex);

    return data;
}

bool spmc_queue_is_empty(struct spmc_queue *queue) {
    return !queue->head->next;
}
