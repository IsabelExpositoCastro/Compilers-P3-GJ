// --------------- LIBRERÍAS ---------------
#include "inArgs.h"

enum {
    REQUIRED_ARG_COUNT = 3,
    PROGRAM_NAME_ARG_INDEX = 0,
    LANGUAGE_ARG_INDEX = 1,
    INPUT_ARG_INDEX = 2,
    BASE_NAME_BUFFER_SIZE = 256,
    OUTPUT_NAME_BUFFER_SIZE = 300
};

static const char* DEFAULT_PROGRAM_NAME = "parser";
static const char* DEFAULT_INPUT_FILENAME = "default_input.cscn";
static const char* OUTPUT_SUFFIX = "_p3dbg.txt";
static const char* INPUT_OPEN_MODE = "r";
static const char* OUTPUT_OPEN_MODE = "w";
static const char* OUTPUT_HEADER_LEGEND = "SHIFT --> S\nREDUCE --> R\nACCEPT --> A\nERROR --> E\n";
static const char* OUTPUT_HEADER_COLUMNS = "OPERATION | STATE | POSITION | INPUT | STACK | ACTION\n";
static const char* OUTPUT_HEADER_SEPARATOR = "\n";




// --------------- FUNCIONES ---------------
int validate_program_args(int argc, char* argv[]) {
    if (argc != REQUIRED_ARG_COUNT) {
        const char* program_name = (argc > PROGRAM_NAME_ARG_INDEX && argv[PROGRAM_NAME_ARG_INDEX] != NULL)
            ? argv[PROGRAM_NAME_ARG_INDEX]
            : DEFAULT_PROGRAM_NAME;
        fprintf(stderr, "Usage: %s <language_file.txt> <input_tokens.cscn>\n", program_name);
        return 0;
    }
    return 1;
}




// GET THE NAME OF THE INPUT FILE
const char* get_input_filename(int argc, char* argv[]) {
    if (argc > INPUT_ARG_INDEX) {
        return argv[INPUT_ARG_INDEX];
    }
    return NULL; 
}

const char* get_language_filename(int argc, char* argv[]) {
    if (argc > LANGUAGE_ARG_INDEX) {
        return argv[LANGUAGE_ARG_INDEX];
    }
    return NULL;
}

int build_output_filename(const char* input_filename, char* output_filename, size_t output_size) {
    char base_name[BASE_NAME_BUFFER_SIZE];
    const char* effective_input = input_filename;
    size_t input_len;
    char* dot;

    if (output_filename == NULL || output_size == 0) {
        return 0;
    }

    if (effective_input == NULL || effective_input[0] == '\0') {
        effective_input = DEFAULT_INPUT_FILENAME;
    }

    input_len = strlen(effective_input);
    if (input_len >= sizeof(base_name)) {
        return 0;
    }

    strcpy(base_name, effective_input);
    dot = strrchr(base_name, '.');
    if (dot != NULL) {
        *dot = '\0';
    }

    if (snprintf(output_filename, output_size, "%s%s", base_name, OUTPUT_SUFFIX) >= (int)output_size) {
        return 0;
    }

    return 1;
}



// OPEN CORRECTLY THE INPUT FILE
FILE* Open_InputFile(int argc, char* argv[]) {
    const char* input_path = get_input_filename(argc, argv);

    if (input_path != NULL) {
        FILE* inputFile = fopen(input_path, INPUT_OPEN_MODE);
        if (!inputFile) {
            fprintf(stderr, "[INPUT_HANDLER] Error: Could not open file %s\n", input_path);
            return NULL;
        }
        return inputFile;
    }
    return NULL;
}

FILE* Open_OutputFile(int argc, char* argv[]) {
    char output_filename[OUTPUT_NAME_BUFFER_SIZE];
    const char* input_filename = get_input_filename(argc, argv);

    if (!build_output_filename(input_filename, output_filename, sizeof(output_filename))) {
        fprintf(stderr, "[OUTPUT_HANDLER] Error: Could not build output filename\n");
        return stdout;
    }

    FILE* output_file = fopen(output_filename, OUTPUT_OPEN_MODE);
    if (!output_file) {
        fprintf(stderr, "[OUTPUT_HANDLER] Error: Could not open output file %s\n", output_filename);
        return stdout;
    }

    fprintf(output_file, "%s", OUTPUT_HEADER_LEGEND);
    fprintf(output_file, "%s", OUTPUT_HEADER_COLUMNS);
    fprintf(output_file, "%s", OUTPUT_HEADER_SEPARATOR);

    return output_file;
}