// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../types.h"

#ifndef LANGUAGE_H
#define LANGUAGE_H

// ------------------- DEFINITIONS -------------------
#define MAX_NAME_LEN     64
#define MAX_RHS_LEN      16
#define MAX_RULES        64
#define MAX_TERMINALS    16
#define MAX_NONTERMINALS 16




// -------------------- DATA STRUCTS -------------------

// HANLDLING THE CONTENT IN RHS OF THE PRODUCTION RULES
typedef struct {            // A symbol in RHS can be terminal or nonterminal
    SymbolKind kind;        // TERMINAL O NONTERMINAL
    union {
        Terminal term;      // CASE_1: IT IS A TERMINAL
        char nonterm;       // CASE_2: IT IS A NONTERMINAL (CHAR REPRESENTING THE NONTERMINAL)
    } val;
} GrammarSymbol;


// PRODUCTION RULE
typedef struct {                           
    char lhs;                              // Left hand side (statement)
    int rhs_len;                           // Length of the right hand side
    GrammarSymbol rhs[MAX_RHS_LEN];        // ARRAY OF SYMBOLS IN RHS
} ProductionRule;


// LANGUAGE
typedef struct {                    
    int num_rules;                          // TOTAL NUMBER OF PRODUCTION RULES                                
    char start_symbol;                      // S
    int num_terminals;                      // TOTAL NUMBER OF TERMINALS
    int num_nonterminals;                   // TOTAL NUMBER OF NONTERMINALS
    ProductionRule rules[MAX_RULES];        // PRODUCTION RULES
    Terminal terminals[MAX_TERMINALS];      // ARRAY DE TERMNALS
    char nonterminals[MAX_NONTERMINALS];    // ARRAY DE NONTERMINALS
} Grammar;



// ------------------- FUNCTIONS -------------------
int load_grammar_from_file(const char* filepath, Grammar* grammar);
Terminal terminal_from_lexeme(const char* lexeme);
const char* terminal_to_string(Terminal terminal);

#endif





