#include "stack.h"
#include "linked_list.h"

void stack_init(struct stack *stack, unsigned long data_size) {
    linked_list_init(&stack->list, data_size);
}

void stack_push(struct stack *stack, void *data) {
    linked_list_insert_beginning(&stack->list, data);
}

void *stack_pop(struct stack *stack) {
    return linked_list_remove_beginning(&stack->list);
}
