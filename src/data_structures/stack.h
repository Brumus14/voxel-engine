#ifndef STACK_H
#define STACK_H

#include "linked_list.h"

struct stack {
    struct linked_list list;
};

void stack_init(struct stack *stack, unsigned long data_size);
void stack_push(struct stack *stack, void *data);
void *stack_pop(struct stack *stack);

#endif
