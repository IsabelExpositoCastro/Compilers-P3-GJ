// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include "../types.h"
#include "../Language_Module/language.h"
#include "../Automata_Module/automata.h"
#include "../Stack_Module/stack.h"
#include "../TokenReader_Module/tokenReader.h"


#ifndef PARSER_H
#define PARSER_H



typedef struct {
    Automaton automaton;
    ParserStack stack;

    Grammar grammar;
    TokenArray tokens;

    int input_pos;
    FILE* dbg_out; //  debug output file
} SRAParser;

#endif



// ------------------- FUNCIONES -------------------
void printCasualParser();