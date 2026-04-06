#include "priority_queue.h"

#include <stdlib.h>
#include "../math/math_util.h"

void priority_queue_init(struct priority_queue *queue) {
    queue->capacity = 1;
    queue->element_count = 0;
    queue->array = malloc(queue->capacity * sizeof(struct priority_queue_node));
}

void priority_queue_destroy(struct priority_queue *queue) {
    free(queue->array);
}

void priority_queue_push(struct priority_queue *queue, void *data,
                         float priority) {
    queue->element_count++;

    if (queue->element_count > queue->capacity) {
        queue->capacity *= 2;
        queue->array = realloc(
            queue->array, queue->capacity * sizeof(struct priority_queue_node));
    }

    struct priority_queue_node node = {data, priority};

    int index = queue->element_count - 1;
    queue->array[index] = node;

    if (index == 0) {
        return;
    }

    int parent_index = floor_div(index - 1, 2);

    while (queue->array[index].priority < queue->array[parent_index].priority) {
        struct priority_queue_node temp = queue->array[parent_index];
        queue->array[parent_index] = queue->array[index];
        queue->array[index] = temp;

        if (parent_index == 0) {
            return;
        }

        index = parent_index;
        parent_index = floor_div(index - 1, 2);
    }
}

void *priority_queue_pop(struct priority_queue *queue) {
}
