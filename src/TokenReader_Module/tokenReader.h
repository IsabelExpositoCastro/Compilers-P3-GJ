// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../types.h"

#ifndef TOKENREADER_H
#define TOKENREADER_H

#define MAX_LEXEME_LEN 64



// ------------------- DATA STRUCTURES -------------------
typedef struct {
    char lexeme[MAX_LEXEME_LEN];
    Terminal category;   // TOK_NUM, TOK_PLUS, ...
} Token;



typedef struct {
    Token* tokens;
    int size;
    int capacity;
} TokenArray;

#endif


// ------------------- FUNCIONES -------------------
void printCasualTokenReader();
void generate_debug_output(const char *input_filename);
void write_step(FILE *f,
                int input_pos,
                const char *operation,
                const char *rule,
                int state,
                const char *stack,
                const char *remaining_input,
                int new_state);