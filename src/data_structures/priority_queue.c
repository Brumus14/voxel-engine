#include "priority_queue.h"

#include <stdbool.h>
#include <stdlib.h>

int parent(int index) {
    return (index - 1) / 2;
}

int left(int index) {
    return 2 * index + 1;
}

int right(int index) {
    return 2 * index + 2;
}

void swap(struct priority_queue_node *a, struct priority_queue_node *b) {
    struct priority_queue_node temp = *a;
    *a = *b;
    *b = temp;
}

void priority_queue_init(struct priority_queue *queue) {
    queue->capacity = 1;
    queue->size = 0;
    queue->array = malloc(queue->capacity * sizeof(struct priority_queue_node));
}

void priority_queue_destroy(struct priority_queue *queue) {
    free(queue->array);
}

bool priority_queue_is_empty(struct priority_queue *queue) {
    return queue->size == 0;
}

void priority_queue_push(struct priority_queue *queue, void *data,
                         float priority) {
    queue->size++;

    if (queue->size > queue->capacity) {
        queue->capacity *= 2;
        queue->array = realloc(
            queue->array, queue->capacity * sizeof(struct priority_queue_node));
    }

    struct priority_queue_node node = {data, priority};

    int index = queue->size - 1;
    queue->array[index] = node;

    while (index != 0 && queue->array[parent(index)].priority >
                             queue->array[index].priority) {
        swap(&queue->array[index], &queue->array[parent(index)]);
        index = parent(index);
    }
}

void *priority_queue_pop(struct priority_queue *queue) {
    if (queue->size == 0) {
        return NULL;
    }

    queue->size--;

    void *data = queue->array[0].data;

    if (queue->size == 0) {
        return data;
    }

    queue->array[0] = queue->array[queue->size];

    int index = 0;

    while (true) {
        int l = left(index);
        int r = right(index);
        int smallest = index;

        if (l < queue->size &&
            queue->array[l].priority < queue->array[index].priority) {
            smallest = l;
        }

        if (r < queue->size &&
            queue->array[r].priority < queue->array[smallest].priority) {
            smallest = r;
        }

        if (smallest == index) {
            break;
        }

        swap(&queue->array[index], &queue->array[smallest]);
        index = smallest;
    }

    return data;
}
