// ------------------ LIBRARIES ------------------
#include "./main.h"
//#define AUTOMATA_FILE "automatas.txt"


// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {
    const char* language_path;
    const char* input_filename;
    Grammar grammar;
    Automaton automaton;
    FILE* ifile;
    FILE* ofile;
    TokenStream stream;
    int parser_ok;

    if (!validate_program_args(argc, argv)) {
        return 1;
    }

    language_path = get_language_filename(argc, argv);
    input_filename = get_input_filename(argc, argv);
    ifile = Open_InputFile(argc, argv);               // OPEN THE INPUT FILE (.cscn)

    if (ifile == NULL) {
        return 1;
    }
    fclose(ifile);

    ofile = Open_OutputFile(argc, argv);
    if (ofile == NULL) {
        return 1;
    }

    printf("[ARGS] input=%s\n", input_filename != NULL ? input_filename : "<stdin>");
    printf("[ARGS] language=%s\n", language_path);
    


    

    // TENEMOS Q CARGAR EL LENGUAGE
    if (!load_grammar_from_file(language_path, &grammar)) {
        fprintf(stderr, "[LANGUAGE] Error loading grammar from %s\n", language_path);
        if (ofile != stdout) fclose(ofile);
        return 1;
    } else {
        printf("[LANGUAGE] Loaded grammar: rules=%d terminals=%d nonterminals=%d\n",
               grammar.num_rules,
               grammar.num_terminals,
               grammar.num_nonterminals);
    }

    if (!load_token_stream_from_file(input_filename, &stream)) {
        fprintf(stderr, "[TOKEN_READER] Error loading token stream from %s\n", input_filename);
        if (ofile != stdout) fclose(ofile);
        return 1;
    }

    if (!automata_load_from_file(language_path, &automaton)) {
        fprintf(stderr, "[AUTOMATA] Error loading ACTION/GOTO tables from %s\n", language_path);
        free_token_stream(&stream);
        if (ofile != stdout) fclose(ofile);
        return 1;
    }

    parser_ok = run_shift_reduce_parser(&grammar, &automaton, &stream, ofile);
    automata_free(&automaton);
    free_token_stream(&stream);

    if (!parser_ok) {
        printf("[PARSER] Completed with one or more rejected expressions. Check debug output file for details.\n");
    } else {
        printf("[PARSER] Completed successfully. All expressions accepted.\n");
    }

    if (ofile != stdout && ofile != NULL) {
        fclose(ofile);
    }
    return 0;

}