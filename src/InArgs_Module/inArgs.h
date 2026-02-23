// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INARGS_H
#define INARGS_H

typedef struct {
    const char* tokens_path;    // input .cscn
    const char* language_path;  // language1.txt / language2.txt
} Args;

#endif


// ------------------- FUNCIONES -------------------
void printCasualInArgs();
FILE* Open_InputFile(int argc, char* argv[]);
FILE* Open_OutputFile(int argc, char* argv[]);
char* get_input_filename(int argc, char* argv[]);


