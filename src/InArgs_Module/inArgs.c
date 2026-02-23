// --------------- LIBRERÍAS ---------------
#include "inArgs.h"




// --------------- FUNCIONES ---------------
void printCasualInArgs() {
    printf("Hellooo desde el InArgs\n");
}




// GET THE NAME OF THE INPUT FILE
char* get_input_filename(int argc, char* argv[]) {
    if (argc > 1) {
        return argv[1];
    }
    return NULL; 
}



// OPEN CORRECTLY THE INPUT FILE
FILE* Open_InputFile(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* inputFile = fopen(argv[1], "r");
        if (!inputFile) {
            fprintf(stderr, "[INPUT_HANDLER] Error: Could not open file %s\n", argv[1]);        // CAMBIAR ESTO POR MANAGEERROR 
            return stdin;
        }
        return inputFile;
    }
    return stdin;
}