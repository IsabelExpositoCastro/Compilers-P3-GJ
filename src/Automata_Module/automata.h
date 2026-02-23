// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include "../types.h"

#ifndef AUTOMATA_H
#define AUTOMATA_H



// Single ACTION cell tiene sentido: shift/reduce/accept/error
typedef struct {
    ActionType type;
    int value; 
} Action;

// Automaton = (Q, Σ, δ, q0, F) pero representado con tablas ACTION y GOTO
typedef struct {
    int num_states;
    int start_state;

    // ACTION[state][terminal] -> Action
    Action** action_table;

    // GOTO[state][nonterminal_index] -> next_state (or -1)
    int** goto_table;

    // Mapping of nonterminal char to a column index for goto_table
    char* goto_nonterminals;
    int num_goto_nonterminals;
} Automaton;

#endif


// ------------------- FUNCIONES -------------------
void printCasualAutomata();