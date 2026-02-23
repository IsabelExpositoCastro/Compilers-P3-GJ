// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#ifndef INARGS_H
#define INARGS_H

typedef struct {
    const char* tokens_path;    // input .cscn
    const char* language_path;  // language1.txt / language2.txt
} Args;

#endif


// ------------------- FUNCIONES -------------------
void printCasualInArgs();
