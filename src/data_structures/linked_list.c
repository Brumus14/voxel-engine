#include "linked_list.h"

#include <stdlib.h>
#include <string.h>

void linked_list_node_init(struct linked_list_node *node, void *data) {
    node->data = data;
    node->previous = NULL;
    node->next = NULL;
}

void linked_list_init(struct linked_list *list, unsigned long data_size) {
    list->head = NULL;
    list->tail = NULL;
    list->data_size = data_size;
}

void linked_list_destroy(struct linked_list *list) {
    struct linked_list_node *node = list->head;

    while (node) {
        struct linked_list_node *next_node = node->next;
        free(node);
        node = next_node;
    }

    list->head = NULL;
    list->tail = NULL;
}

void linked_list_insert_beginning(struct linked_list *list, void *data) {
    struct linked_list_node *new_node = malloc(sizeof(struct linked_list_node));
    linked_list_node_init(new_node, data);

    new_node->next = list->head;

    if (list->head) {
        list->head->previous = new_node;
    } else {
        list->tail = new_node;
    }

    list->head = new_node;
}

void linked_list_insert_end(struct linked_list *list, void *data) {
    struct linked_list_node *new_node = malloc(sizeof(struct linked_list_node));
    linked_list_node_init(new_node, data);

    new_node->previous = list->tail;

    if (list->tail) {
        list->tail->next = new_node;
    } else {
        list->head = new_node;
    }

    list->tail = new_node;
}

void linked_list_insert(struct linked_list *list, void *data, int index) {
    int list_length = linked_list_length(list);

    if (index > list_length) {
        return;
    }

    if (index == 0) {
        linked_list_insert_beginning(list, data);
        return;
    } else if (index == list_length) {
        linked_list_insert_end(list, data);
    } else {
        struct linked_list_node *new_node =
            malloc(sizeof(struct linked_list_node));
        linked_list_node_init(new_node, data);

        struct linked_list_node *node = list->head;

        for (int i = 0; i < index; i++) {
            node = node->next;
        }

        if (!node) {
            linked_list_insert_end(list, data);
            return;
        }

        new_node->previous = node->previous;
        new_node->next = node;
        node->previous->next = new_node;
        node->previous = new_node;
    }
}

void *linked_list_remove_beginning(struct linked_list *list) {
    if (!list->head) {
        return NULL;
    }

    struct linked_list_node *removed_node = list->head;
    list->head = list->head->next;

    if (list->head) {
        list->head->previous = NULL;
    } else {
        list->tail = NULL;
    }

    void *removed_data = removed_node->data;

    free(removed_node);

    return removed_data;
}

void *linked_list_remove_end(struct linked_list *list) {
    if (!list->head) {
        return NULL;
    }

    struct linked_list_node *removed_node = list->tail;
    list->tail = list->tail->previous;

    if (list->tail) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    void *removed_data = removed_node->data;

    free(removed_node);

    return removed_data;
}

void *linked_list_remove(struct linked_list *list, int index) {
    int list_length = linked_list_length(list);

    if (index >= list_length) {
        return NULL;
    }

    if (index == 0) {
        return linked_list_remove_beginning(list);
    } else if (index == list_length - 1) {
        return linked_list_remove_end(list);
    } else {
        struct linked_list_node *removed_node = list->head;

        for (int i = 0; i < index; i++) {
            removed_node = removed_node->next;
        }

        removed_node->previous->next = removed_node->next;
        removed_node->next->previous = removed_node->previous;

        void *removed_data = removed_node->data;

        free(removed_node);

        return removed_data;
    }
}

unsigned int linked_list_length(struct linked_list *list) {
    int length = 0;
    struct linked_list_node *current_node = list->head;

    while (current_node) {
        length++;
        current_node = current_node->next;
    }

    return length;
}

void *linked_list_get(struct linked_list *list, int index) {
    if (index >= linked_list_length(list)) {
        return NULL;
    }

    struct linked_list_node *current_node = list->head;

    for (int i = 0; i < index; i++) {
        current_node = current_node->next;
    }

    return current_node->data;
}

bool linked_list_is_empty(struct linked_list *list) {
    return !list->head;
}

unsigned int linked_list_find(struct linked_list *list, void *data) {
    struct linked_list_node *current_node = list->head;
    int index = 0;

    while (current_node != list->tail &&
           memcmp(current_node->data, data, list->data_size) == 0) {
        current_node = current_node->next;
        index++;
    }

    if (current_node == list->tail) {
        return -1;
    }

    return index;
}
