#include "dynamic_array.h"

#include <stdlib.h>
#include <string.h>

#define GROWTH_FACTOR 2

void dynamic_array_init(struct dynamic_array *array, unsigned long data_size) {
    array->array = malloc(data_size);
    array->capacity = 1;
    array->element_count = 0;
    array->value_size = data_size;
}

void dynamic_array_destroy(struct dynamic_array *array) {
    free(array->array);
}

void *dynamic_array_get(struct dynamic_array *array, unsigned long index) {
    return array->array + index * array->value_size;
}

void dynamic_array_set(struct dynamic_array *array, unsigned long index,
                       void *data) {
    // Use indexing maybe?
    memcpy(array->array + index * array->value_size, data, array->value_size);
}

void dynamic_array_insert_end(struct dynamic_array *array, void *data) {
    array->element_count++;

    // Grow array
    if (array->element_count > array->capacity) {
        array->capacity *= GROWTH_FACTOR; // Growth factor might be decimal
        array->array =
            realloc(array->array, array->capacity * array->value_size);
    }

    memcpy(array->array + (array->element_count - 1) * array->value_size, data,
           array->value_size);
}

// Should this shrink the array?
// Should this return removed value?
// This isnt very safe
void *dynamic_array_remove_end(struct dynamic_array *array) {
    array->element_count--;
    return array->array + array->element_count * array->value_size;
}

void dynamic_array_clear(struct dynamic_array *array) {
    array->element_count = 0;
}
