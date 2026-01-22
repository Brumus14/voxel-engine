#ifndef QUEUE_H
#define QUEUE_H

#include "linked_list.h"

struct queue {
    struct linked_list list;
};

void queue_init(struct queue *queue, unsigned long data_size);
void queue_destroy(struct queue *queue);
void queue_enqueue(struct queue *queue, void *data);
void *queue_dequeue(struct queue *queue);
bool queue_is_empty(struct queue *queue);
unsigned int queue_length(struct queue *queue);
void *queue_get(struct queue *queue, int index);
unsigned int queue_find(struct queue *queue, void *data);

#endif
