#include "queue.h"

#include <stddef.h>

void queue_init(struct queue *queue, unsigned long data_size) {
    linked_list_init(&queue->list, data_size);
}

void queue_destroy(struct queue *queue) {
    while (!queue_is_empty(queue)) {
        queue_dequeue(queue);
    }
}

void queue_enqueue(struct queue *queue, void *data) {
    linked_list_insert_end(&queue->list, data);
}

void *queue_dequeue(struct queue *queue) {
    return linked_list_remove_beginning(&queue->list);
}

bool queue_is_empty(struct queue *queue) {
    return linked_list_is_empty(&queue->list);
}

unsigned int queue_length(struct queue *queue) {
    return linked_list_length(&queue->list);
}

void *queue_get(struct queue *queue, int index) {
    return linked_list_get(&queue->list, index);
}

unsigned int queue_find(struct queue *queue, void *data) {
    return linked_list_find(&queue->list, data);
}
