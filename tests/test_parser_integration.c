#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "../src/Language_Module/language.h"
#include "../src/TokenReader_Module/tokenReader.h"
#include "../src/Parser_Module/parser.h"

#ifndef PROJECT_ROOT
#define PROJECT_ROOT "."
#endif

static void build_path(char* out, size_t out_size, const char* relative) {
    snprintf(out, out_size, "%s/%s", PROJECT_ROOT, relative);
}

int main(void) {
    Grammar grammar;
    Automaton automaton;
    TokenStream stream;
    char language1_path[512];
    char language2_path[512];
    char input1_path[512];
    char input2_path[512];
    FILE* debug_output;

    build_path(language1_path, sizeof(language1_path), "data/language.txt");
    build_path(language2_path, sizeof(language2_path), "data/language2.txt");
    build_path(input1_path, sizeof(input1_path), "data/input_valid_only.cscn");
    build_path(input2_path, sizeof(input2_path), "data/input_lang2_valid.cscn");

    debug_output = tmpfile();
    assert(debug_output != NULL);

    assert(load_grammar_from_file(language1_path, &grammar) == 1);
    assert(automata_load_from_file(language1_path, &automaton) == 1);
    assert(load_token_stream_from_file(input1_path, &stream) == 1);
    assert(run_shift_reduce_parser(&grammar, &automaton, &stream, debug_output) == 1);
    free_token_stream(&stream);
    automata_free(&automaton);

    assert(load_grammar_from_file(language2_path, &grammar) == 1);
    assert(automata_load_from_file(language2_path, &automaton) == 1);
    assert(load_token_stream_from_file(input2_path, &stream) == 1);
    assert(run_shift_reduce_parser(&grammar, &automaton, &stream, debug_output) == 1);
    free_token_stream(&stream);
    automata_free(&automaton);

    fclose(debug_output);
    return 0;
}
