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
// ------------------- FUNCIONES -------------------
int run_shift_reduce_parser(const Grammar* grammar, const Automaton* automaton, const TokenStream* stream, FILE* dbg_out);

#endif