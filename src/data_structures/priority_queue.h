#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

// Implemented using a min-heap

struct priority_queue_node {
    void *data;
    float priority;
};

struct priority_queue {
    int capacity;
    int element_count;
    struct priority_queue_node *array;
};

void priority_queue_init(struct priority_queue *queue);
void priority_queue_destroy(struct priority_queue *queue);
void priority_queue_push(struct priority_queue *queue, void *data,
                         float priority);
void *priority_queue_pop(struct priority_queue *queue);

#endif
