// --------------- LIBRERÍAS ---------------
#include "inArgs.h"




// --------------- FUNCIONES ---------------
int validate_program_args(int argc, char* argv[]) {
    if (argc != 3) {
        const char* program_name = (argc > 0 && argv[0] != NULL) ? argv[0] : "parser";
        fprintf(stderr, "Usage: %s <language_file.txt> <input_tokens.cscn>\n", program_name);
        return 0;
    }
    return 1;
}




// GET THE NAME OF THE INPUT FILE
const char* get_input_filename(int argc, char* argv[]) {
    if (argc > 2) {
        return argv[2];
    }
    return NULL; 
}

const char* get_language_filename(int argc, char* argv[]) {
    if (argc > 1) {
        return argv[1];
    }
    return NULL;
}

int build_output_filename(const char* input_filename, char* output_filename, size_t output_size) {
    char base_name[256];
    const char* effective_input = input_filename;
    size_t input_len;
    char* dot;

    if (output_filename == NULL || output_size == 0) {
        return 0;
    }

    if (effective_input == NULL || effective_input[0] == '\0') {
        effective_input = "default_input.cscn";
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

    if (snprintf(output_filename, output_size, "%s_p3dbg.txt", base_name) >= (int)output_size) {
        return 0;
    }

    return 1;
}



// OPEN CORRECTLY THE INPUT FILE
FILE* Open_InputFile(int argc, char* argv[]) {
    const char* input_path = get_input_filename(argc, argv);

    if (input_path != NULL) {
        FILE* inputFile = fopen(input_path, "r");
        if (!inputFile) {
            fprintf(stderr, "[INPUT_HANDLER] Error: Could not open file %s\n", input_path);
            return NULL;
        }
        return inputFile;
    }
    return NULL;
}

FILE* Open_OutputFile(int argc, char* argv[]) {
    char output_filename[300];
    const char* input_filename = get_input_filename(argc, argv);

    if (!build_output_filename(input_filename, output_filename, sizeof(output_filename))) {
        fprintf(stderr, "[OUTPUT_HANDLER] Error: Could not build output filename\n");
        return stdout;
    }

    FILE* output_file = fopen(output_filename, "w");
    if (!output_file) {
        fprintf(stderr, "[OUTPUT_HANDLER] Error: Could not open output file %s\n", output_filename);
        return stdout;
    }

    return output_file;
}