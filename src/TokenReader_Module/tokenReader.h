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

typedef struct {
    TokenArray* expressions;
    int size;
    int capacity;
} TokenStream;

#endif


// ------------------- FUNCIONES -------------------
int load_token_stream_from_file(const char* input_path, TokenStream* stream);
void free_token_stream(TokenStream* stream);
void write_step(FILE *f,
                int input_pos,
                const char *operation,
                int state,
                const char *stack,
                const char *remaining_input,
                const char *action);