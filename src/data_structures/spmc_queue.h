#ifndef SPMC_QUEUE_H
#define SPMC_QUEUE_H

// A SPMC (Single Producer, Multiple Consumer) queue
// Using a sentinel node to avoid edge cases

#include "stdbool.h"
#include <pthread.h>

struct spmc_queue_node {
    void *data;
    struct spmc_queue_node *next;
};

void spmc_queue_node_init(struct spmc_queue_node *node, void *data);

struct spmc_queue {
    struct spmc_queue_node *head;
    struct spmc_queue_node *tail;
    pthread_mutex_t head_mutex;
};

void spmc_queue_init(struct spmc_queue *queue);
void spmc_queue_destroy(struct spmc_queue *queue);
void spmc_queue_enqueue(struct spmc_queue *queue, void *data);
void *spmc_queue_dequeue(struct spmc_queue *queue);
bool spmc_queue_is_empty(struct spmc_queue *queue);

#endif
