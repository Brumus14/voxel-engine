#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "stdbool.h"

struct linked_list_node {
    void *data;
    struct linked_list_node *previous;
    struct linked_list_node *next;
};

void linked_list_node_init(struct linked_list_node *node, void *data);

struct linked_list {
    struct linked_list_node *head;
    struct linked_list_node *tail;
    unsigned long data_size;
};

void linked_list_init(struct linked_list *list, unsigned long data_size);
void linked_list_destroy(struct linked_list *list);
void linked_list_insert_beginning(struct linked_list *list, void *data);
void linked_list_insert_end(struct linked_list *list, void *data);
void linked_list_insert(struct linked_list *list, void *data, int index);
void *linked_list_remove_beginning(struct linked_list *list);
void *linked_list_remove_end(struct linked_list *list);
void *linked_list_remove(struct linked_list *list, int index);
unsigned int linked_list_length(struct linked_list *list);
void *linked_list_get(struct linked_list *list, int index);
bool linked_list_is_empty(struct linked_list *list);
unsigned int linked_list_find(struct linked_list *list, void *data);

#endif
