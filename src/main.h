// ------------------- LIBRERÍAS -------------------
#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>

#include "types.h"
#include "./utils_files.h"
#include "./Stack_Module/stack.h"
#include "./Parser_Module/parser.h"
#include "./InArgs_Module/inArgs.h"
#include "./Language_Module/language.h"
#include "./Automata_Module/automata.h"
#include "./TokenReader_Module/tokenReader.h"


// Output file of project run: either a stdout or a filename with log extension (comment one out)
#define PROJOUTFILENAME "./bottom_up.log"
//#define PROJOUTFILENAME "stdout"

extern FILE* ofile; // The output handler for the project run (same variable name as in modules)

#endif // MAIN_H
