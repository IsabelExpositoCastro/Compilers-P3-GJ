// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../types.h"

#ifndef LANGUAGE_H
#define LANGUAGE_H


#define MAX_NAME_LEN     64
#define MAX_RHS_LEN      16
#define MAX_RULES        64
#define MAX_TERMINALS    16
#define MAX_NONTERMINALS 16

// A symbol in RHS can be terminal or nonterminal
typedef struct {
    SymbolKind kind;
    union {
        Terminal term;
        char nonterm;
    } val;
} GrammarSymbol;

// Production rule: lhs -> rhs[0..rhs_len-1]
typedef struct {
    char lhs;                      // ex 'e'
    int rhs_len;                   // number of symbols in RHS
    GrammarSymbol rhs[MAX_RHS_LEN];
} ProductionRule;

// Grammar = (V, Σ, P, S)
typedef struct {
    char name[MAX_NAME_LEN];

    char start_symbol;

    Terminal terminals[MAX_TERMINALS];
    int num_terminals;

    char nonterminals[MAX_NONTERMINALS];
    int num_nonterminals;

    ProductionRule rules[MAX_RULES];
    int num_rules;
} Grammar;

void printCasualLanguage();

#endif





