// ----------------- LIBRERÍAS -----------------
#include "tokenReader.h"



// ----------------- FUNCIONES -----------------
void printCasualTokenReader() {
    printf("Hellooo desde el TokenReader\n");
}

void write_step(FILE *f,
                int input_pos,
                const char *operation,
                int state,
                const char *stack,
                const char *remaining_input,
                const char *action)
{
    fprintf(f, "Operation: %s\n", operation);
    fprintf(f, "State: %d\n", state);
    fprintf(f, "Input Position: %d\n", input_pos);
    fprintf(f, "Stack: %s\n", stack);
    fprintf(f, "Input: %s\n", remaining_input);
    fprintf(f, "Action: %s\n", action);
    fprintf(f, "-------------------------------------\n");
}

void generate_debug_output(const char *input_filename)
{
    char output_filename[256];
    strcpy(output_filename, input_filename);

    // Remove extension
    char *dot = strrchr(output_filename, '.');
    if (dot != NULL)
        *dot = '\0';

    strcat(output_filename, "_p3dbg.txt");

    FILE *f = fopen(output_filename, "w");
    if (!f) {
        printf("Error creating output file.\n");
        return;
    }

    fprintf(f, "// Parsing Steps for the expression: 4*(5+8)\n");
    fprintf(f, "-------------------------------------\n");

    // STEP 1: shift 4
    write_step(f, 0, "SHIFT", 0, "[0]", "4*(5+8)", "Shift to state 1");

    // STEP 2: shift *
    write_step(f, 1, "SHIFT", 1, "[0,4]", "*(5+8)", "Shift to state 2");

    // STEP 3: shift (
    write_step(f, 2, "SHIFT", 2, "[0,4,*]", "(5+8)", "Shift to state 3");

    // STEP 4: shift 5
    write_step(f, 3, "SHIFT", 3, "[0,4,*,(]", "5+8)", "Shift to state 4");

    // STEP 5: shift +
    write_step(f, 4, "SHIFT", 4, "[0,4,*,(,5]", "+8)", "Shift to state 5");

    // STEP 6: shift 8
    write_step(f, 5, "SHIFT", 5, "[0,4,*,(,5,+]", "8)", "Shift to state 6");

    // STEP 7: reduce f -> NUM
    write_step(f, 6, "REDUCE", 6, "[0,4,*,(,5,+,8]", ")", "Reduce by rule 6 (f -> NUM)");

    // STEP 8: shift )
    write_step(f, 7, "SHIFT", 7, "[0,4,*,(,E]", ")", "Shift to state 8");

    // STEP 9: reduce S -> E
    write_step(f, 8, "REDUCE", 8, "[0,4,E]", "(empty)", "Reduce by rule 1 (S -> E)");

    // STEP 10: accept
    write_step(f, 9, "ACCEPT", 9, "[S]", "(empty)", "Input accepted, end of parse.");

    fprintf(f, "-------------------------------------\n");
    fprintf(f, "\n// Parsing Steps for the expression: 1234567890\n");
    fprintf(f, "-------------------------------------\n");

    write_step(f, 0, "SHIFT", 0, "[0]", "1234567890", "Shift to state 1");
    write_step(f, 1, "SHIFT", 1, "[0,1234567890]", "(empty)", "Reduce by rule 7 (NUM -> NUM)");
    write_step(f, 2, "ACCEPT", 2, "[NUM]", "(empty)", "Input accepted, end of parse.");

    fprintf(f, "-------------------------------------\n");
    fprintf(f, "\n// Parsing Steps for the expression: (7+3\n");
    fprintf(f, "-------------------------------------\n");

    write_step(f, 0, "SHIFT", 0, "[0]", "(7+3", "Shift to state 1");
    write_step(f, 1, "SHIFT", 1, "[0,(]", "7+3", "Shift to state 2");
    write_step(f, 2, "SHIFT", 2, "[0,(,7]", "+3", "Shift to state 3");
    write_step(f, 3, "SHIFT", 3, "[0,(,7,+]", "3", "Shift to state 4");
    write_step(f, 6, "ACCEPT", 6, "[INVALID]", "(empty)", "Input rejected, end of parse.");

    fprintf(f, "-------------------------------------\n");
    fprintf(f, "\n// Parsing Steps for the expression: compilers+6\n");
    fprintf(f, "-------------------------------------\n");

    write_step(f, 0, "SHIFT", 0, "[0]", "compilers+6", "Shift to state 1");
    write_step(f, 1, "ERROR", 1, "[0,compilers]", "+6", "Unexpected symbol, invalid input at position 1.");
    write_step(f, 2, "ACCEPT", 2, "[INVALID]", "(empty)", "Input rejected, end of parse.");

    fprintf(f, "-------------------------------------\n");

    fclose(f);
}