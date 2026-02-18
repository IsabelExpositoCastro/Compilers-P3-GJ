#ifndef MAIN_H
#define MAIN_H
 
#include <stdio.h>
#include "./utils_files.h"

// Output file of project run: either a stdout or a filename with log extension (comment one out)
#define PROJOUTFILENAME "./bottom_up.log"
//#define PROJOUTFILENAME "stdout"

extern FILE* ofile; // The output handler for the project run (same variable name as in modules)

#endif // MAIN_H
