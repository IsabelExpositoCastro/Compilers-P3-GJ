// ------------------ LIBRARIES ------------------
#include <time.h>
#include "./utils_files.h"
#include "./Parser_Module/parser.h"

enum {
    YEAR_BASE = 1900,
    MONTH_BASE = 1
};

static const char* DEFAULT_LOG_EXTENSION = "log";
static const char* STDOUT_TARGET = "stdout";
static const char* APPEND_MODE = "a";
static const char* TZ_SETTING = "TZ=GMT-2";
static const char* MSG_MACHINE_REMOTE_TIME = "Machine remote time ";
static const char* ERR_OUTPUT_OPEN = "Error opening output file %s. Check if subdirectory exists, otherwise create it and run again\n";
static const char* MSG_STDOUT_LOG = "See log of execution in stdout (filename %s)\n";
static const char* MSG_FILE_LOG = "See log of execution in file %s\n";
static const char* FALLBACK_INPUT_LABEL = "<stdin>";
static const char* LOG_ARGS_INPUT = "[ARGS] input=%s\n";
static const char* LOG_ARGS_LANGUAGE = "[ARGS] language=%s\n";
static const char* LOG_LANGUAGE_LOADED = "[LANGUAGE] Loaded grammar: rules=%d terminals=%d nonterminals=%d\n";
static const char* LOG_PARSER_DONE = "[PARSER] Execution finished.\n";




// ---------------- FUNCTIONS ------------------
void split_path(const char *fullpath, char *path, char *filename, char *extension) {
    const char *last_slash = strrchr(fullpath, '/');
    const char *last_dot = strrchr(fullpath, '.');

    if (last_slash) {
        size_t path_len = last_slash - fullpath + 1;
        strncpy(path, fullpath, path_len);
        path[path_len] = '\0';
        strcpy(filename, last_slash + 1);
    } else {
        path[0] = '\0';
        strcpy(filename, fullpath);
    }

    if (last_dot) {
        strcpy(extension, last_dot + 1);
        /* remove extension from filename: compute index relative to filename start */
        const char *name_start = (last_slash ? last_slash + 1 : fullpath);
        if (last_dot > name_start) {
            size_t name_len = (size_t)(last_dot - name_start);
            filename[name_len] = '\0';
        }
    } else {
        extension[0] = '\0';
    }
}




void generate_timestamped_log_filename(const char* base_name, char* output, size_t maxlen) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char path[MAXFILENAME], filename[MAXFILENAME], extension[MAXFILEEXT];

    split_path(base_name, path, filename, extension);

    if (extension[0] == '\0') {
        snprintf(extension, sizeof(extension), "%s", DEFAULT_LOG_EXTENSION);
    }

    // Format: yyyy_mm_dd_hh_mm_base
    snprintf(output, maxlen, "%s%04d_%02d_%02d_%02d_%02d_%s.%s",
             PATHDIRLOGS, // path
             t->tm_year + YEAR_BASE,
             t->tm_mon + MONTH_BASE,
             t->tm_mday,
             t->tm_hour,
             t->tm_min, 
             filename,
             extension);
}







// Function to set the output file for test results
// If the filename is "stdout", it will use stdout, otherwise it will open the specified filename
// It adds the timestamp to the filename if it is not "stdout"
FILE* set_output_test_file(const char* filename) {
    FILE *ofile = stdout;
    char timestamped_filename[MAXFILENAME];

    if (strcmp(filename, STDOUT_TARGET) != 0) {
        fprintf(ofile, "%s", MSG_MACHINE_REMOTE_TIME);
        generate_timestamped_log_filename(filename, timestamped_filename, sizeof(timestamped_filename));

        // Set the time zone to Europe/Madrid: 
        // (i.e. fake it as GMT-3 if Madrid is in GMT+2 summer time)
        // When run in github actions the server is in another time zone
        // We want timestamp related to our time
        _putenv(TZ_SETTING);
        //_putenv("TZ=Europe/Madrid");
        _tzset();
        generate_timestamped_log_filename(filename, timestamped_filename, sizeof(timestamped_filename));
        filename = timestamped_filename;

        ofile = fopen(filename, APPEND_MODE); // Tasks can be fast, so they are appended to the same file if it is the same minute
        if (ofile == NULL) {
            fprintf(stderr, ERR_OUTPUT_OPEN, filename);
            ofile = stdout;
        }
    }
    if(ofile == stdout){
        printf(MSG_STDOUT_LOG, filename);
        fprintf(ofile, MSG_STDOUT_LOG, filename);
    }
    else{
        printf(MSG_FILE_LOG, filename);
        fprintf(ofile, MSG_FILE_LOG, filename);
    }
    fflush(ofile);
    return ofile;
}

void print_run_context(const char* input_filename, const char* language_path) {
    printf(LOG_ARGS_INPUT, input_filename != NULL ? input_filename : FALLBACK_INPUT_LABEL);
    printf(LOG_ARGS_LANGUAGE, language_path);
}

void print_grammar_info(const Grammar* grammar) {
    printf(LOG_LANGUAGE_LOADED,
           grammar->num_rules,
           grammar->num_terminals,
           grammar->num_nonterminals);
}

void load_pipeline(const char* language_path,
                   const char* input_filename,
                   Grammar* grammar,
                   TokenStream* stream,
                   Automaton* automaton) {
    load_grammar_from_file(language_path, grammar);
    load_token_stream_from_file(input_filename, stream);
    automata_load_from_file(language_path, automaton);
}

void execute_pipeline(const Grammar* grammar,
                      const Automaton* automaton,
                      const TokenStream* stream,
                      FILE* output_file) {
    run_shift_reduce_parser(grammar, automaton, stream, output_file);
    printf("%s", LOG_PARSER_DONE);
}

void release_pipeline(TokenStream* stream, Automaton* automaton, FILE* output_file) {
    automata_free(automaton);
    free_token_stream(stream);
    fclose(output_file);
}

