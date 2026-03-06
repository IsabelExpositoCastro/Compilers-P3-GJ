#ifndef UTILS_FILES_H
#define UTILS_FILES_H

// ------------------ LIBRARIES ------------------
#include <stdio.h>
#include <stdlib.h>     // To set env for time zone
#include <string.h>     // For filename manipulation    
#include <time.h>       // For time-stamp of output logs filenames
#include "./Language_Module/language.h"
#include "./Automata_Module/automata.h"
#include "./TokenReader_Module/tokenReader.h"


// ---------------- GLOBAL DEFINES ------------------
#define MAXFILENAME 256 // Maximum length of the filename for output logs
#define MAXFILEEXT 64   // Maximum length of the file extension

// Path to the logs directory: put your full path, the directory has to exist
//#define PATHDIRLOGS "I:/Mi unidad/UPFdrive/docencia/github/compilers/modules_template/logs/" 
#define PATHDIRLOGS "./logs/" // For running yml

// Function prototypes
void split_path(const char *fullpath, char *path, char *filename, char *extension);
void generate_timestamped_log_filename(const char* base_name, char* output, size_t maxlen);
FILE* set_output_test_file(const char* filename);
void print_run_context(const char* input_filename, const char* language_path);
void print_grammar_info(const Grammar* grammar);
void load_pipeline(const char* language_path,
				   const char* input_filename,
				   Grammar* grammar,
				   TokenStream* stream,
				   Automaton* automaton);
void execute_pipeline(const Grammar* grammar,
					  const Automaton* automaton,
					  const TokenStream* stream,
					  FILE* output_file);
void release_pipeline(TokenStream* stream, Automaton* automaton, FILE* output_file);

#endif // UTILS_FILES_H