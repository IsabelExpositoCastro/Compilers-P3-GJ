// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include "../types.h"
#include "../Language_Module/language.h"

#ifndef AUTOMATA_H
#define AUTOMATA_H


// ------------------- DATA STRUCTURES -------------------
// Single ACTION cell tiene sentido: shift/reduce/accept/error
// 
typedef struct {
    ActionType type;    // SHIFT, REDUCE, ACCEPT, ERROR
    int value;          // {ACT_SHIFT, shiftear al estado x} - {ACT_REDUCE, production rule número y} 
} Action;



// Automaton = (Q, Σ, δ, q0, F) pero representado con tablas ACTION y GOTO
typedef struct {
    int num_states;             // TOTAL NUMBER OF STATES
    int start_state;            // INTIAL STATE (q0)
    
    Action** action_table;      // ACTION [state][terminal] -> Action
    int** goto_table;           // GOTO [state][nonterminal_index] -> next_state (or -1)

    // Mapping of nonterminal char to a column index for goto_table
    char* goto_nonterminals;
    int num_goto_nonterminals;
} Automaton;

#endif


// ------------------- FUNCIONES -------------------
int automata_load_from_file(const char* language_path, Automaton* automaton);
void automata_free(Automaton* automaton);
Action automata_get_action(const Automaton* automaton, int state, Terminal terminal);
int automata_get_goto(const Automaton* automaton, int state, char nonterminal);