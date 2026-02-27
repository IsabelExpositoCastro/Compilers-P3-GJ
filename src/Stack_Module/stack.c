// ----------------- LIBRERÍAS -----------------
#include "stack.h"




// ----------------- FUNCIONES -----------------
void stack_init(ParserStack* stack) {
    if (stack == NULL) return;
    stack->size = 0;
    stack->capacity = 16;
    stack->data = (StackItem*)malloc(sizeof(StackItem) * stack->capacity);
}

void stack_free(ParserStack* stack) {
    if (stack == NULL) return;
    free(stack->data);
    stack->data = NULL;
    stack->size = 0;
    stack->capacity = 0;
}

int stack_push(ParserStack* stack, StackItem item) {
    StackItem* resized_data = NULL;

    if (stack == NULL || stack->data == NULL) return 0;

    if (stack->size >= stack->capacity) {
        stack->capacity *= 2;
        resized_data = (StackItem*)realloc(stack->data, sizeof(StackItem) * stack->capacity);
        if (resized_data == NULL) {
            return 0;
        }
        stack->data = resized_data;
    }

    stack->data[stack->size++] = item;
    return 1;
}

int stack_pop(ParserStack* stack, StackItem* out_item) {
    if (stack == NULL || stack->size <= 0) return 0;

    if (out_item != NULL) {
        *out_item = stack->data[stack->size - 1];
    }
    stack->size--;
    return 1;
}

int stack_top_state(const ParserStack* stack) {
    if (stack == NULL || stack->size <= 0) return -1;
    return stack->data[stack->size - 1].state;
}