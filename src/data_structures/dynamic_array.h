#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

struct dynamic_array {
    unsigned long capacity;
    void *array;
    unsigned long element_count;
    unsigned long value_size;
};

void dynamic_array_init(struct dynamic_array *array, unsigned long data_size);
void dynamic_array_destroy(struct dynamic_array *array);
void *dynamic_array_get(struct dynamic_array *array, unsigned long index);
void dynamic_array_set(struct dynamic_array *array, unsigned long index,
                       const void *value);
void dynamic_array_insert_end(struct dynamic_array *array, const void *value);
void *dynamic_array_remove_end(struct dynamic_array *array);
void dynamic_array_clear(struct dynamic_array *array);
void dynamic_array_insert_set_end(struct dynamic_array *array, const void *set,
                                  int count);

#endif
