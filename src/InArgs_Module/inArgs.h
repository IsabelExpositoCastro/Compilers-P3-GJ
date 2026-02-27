// ------------------- LIBRERÍAS -------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef INARGS_H
#define INARGS_H

#endif


// ------------------- FUNCIONES -------------------
int validate_program_args(int argc, char* argv[]);
FILE* Open_InputFile(int argc, char* argv[]);
FILE* Open_OutputFile(int argc, char* argv[]);
const char* get_input_filename(int argc, char* argv[]);
const char* get_language_filename(int argc, char* argv[]);
int build_output_filename(const char* input_filename, char* output_filename, size_t output_size);


