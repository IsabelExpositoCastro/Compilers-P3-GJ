// ------------------ LIBRARIES ------------------
#include "./main.h"
#include "./utils_files.h"


enum {
    EXIT_OK = 0
};


// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    const char* language_path = get_language_filename(argc, argv);
    const char* input_filename = get_input_filename(argc, argv);
    Grammar grammar;
    Automaton automaton;
    FILE* ifile = Open_InputFile(argc, argv);
    FILE* ofile = Open_OutputFile(argc, argv);
    TokenStream stream;

    validate_program_args(argc, argv);
    fclose(ifile);
    print_run_context(input_filename, language_path);

    load_pipeline(language_path, input_filename, &grammar, &stream, &automaton);
    print_grammar_info(&grammar);
    execute_pipeline(&grammar, &automaton, &stream, ofile);
    release_pipeline(&stream, &automaton, ofile);

    return EXIT_OK;
}
