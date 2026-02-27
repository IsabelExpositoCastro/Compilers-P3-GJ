// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include "../types.h"

#ifndef STACK_H
#define STACK_H




// ------------------- DATA STRUCTURES -------------------
typedef struct {
    SymbolKind kind; // terminal or nonterminal
    union {
        Terminal term;
        char nonterm;
    } sym;

    int state; // LR state associated after pushing this symbol 
} StackItem;



typedef struct {
    StackItem* data;
    int size;
    int capacity;
} ParserStack;

#endif



// ------------------- FUNCIONES -------------------
void stack_init(ParserStack* stack);
void stack_free(ParserStack* stack);
int stack_push(ParserStack* stack, StackItem item);
int stack_pop(ParserStack* stack, StackItem* out_item);
int stack_top_state(const ParserStack* stack);