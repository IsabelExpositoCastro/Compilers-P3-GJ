// ------------------ LIBRARIES ------------------
#include "./main.h"
//#define AUTOMATA_FILE "automatas.txt"


// ------------------ MAIN FUNCTION ------------------
int main(int argc, char *argv[]) {

    FILE* autfile = fopen("/data/language.txt", "r");       // OPEN THE LANGUAGE.txt file
    FILE* ifile = Open_InputFile(argc, argv);               // OPEN THE INPUT FILE (.cscn)              
    // FILE* ofile = Open_OutputFile(argc, argv);           // OPEN THE OUTPUT FILE (donde printearemos la tabla resultante)
    char* input_filename = get_input_filename(argc, argv);  // GETS THE InputFile NAME
    generate_debug_output(input_filename);                  //Dummy function to generate an visual like output file
    


    

    // TENEMOS Q CARGAR EL LENGUAGE 

    //Llamada a argumentos, sera copiar pegar de la anteriror.
    printf("HERE WE GO AGAIN\n");
    printCasualInArgs();
    printCasualLanguage();
    printCasualAutomata();
    printCasualTokenReader();
    printCasualStack();
    printCasualParser();
    return 0;

}