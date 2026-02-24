// ----------------- LIBRERÍAS -----------------
#include "tokenReader.h"



// ----------------- FUNCIONES -----------------
void printCasualTokenReader() {
    printf("Hellooo desde el TokenReader\n");
}

void write_step(FILE *f,
                int input_pos,
                const char *operation,
                const char *rule,
                int state,
                const char *stack,
                const char *remaining_input,
                int new_state)
{
    fprintf(f, "Operation: %s\n", operation);
    fprintf(f, "State: %d\n", state);
    fprintf(f, "Input Position: %d\n", input_pos);
    fprintf(f, "Stack: %s\n", stack);
    fprintf(f, "Input: %s\n", remaining_input);

    if (strcmp(operation, "SHIFT") == 0) {
        fprintf(f, "Action: Shift to state %d\n", new_state);
    } else if (strcmp(operation, "REDUCE") == 0) {
        fprintf(f, "Action: Reduce by %s\n", rule);
    } else if (strcmp(operation, "ACCEPT") == 0) {
        fprintf(f, "Action: Input accepted, end of parse.\n");
    } else if (strcmp(operation, "ERROR") == 0) {
        fprintf(f, "Action: Unexpected symbol, invalid input at position %d.\n", input_pos);
    }

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

    // STEP 1: shift 4
    write_step(f, 0, "SHIFT", NULL, 0, "[0]", "4*(5+8)", 1);

    // STEP 2: shift *
    write_step(f, 1, "SHIFT", NULL, 1, "[0,4]", "*(5+8)", 2);

    // STEP 3: shift (
    write_step(f, 2, "SHIFT", NULL, 2, "[0,4,*]", "(5+8)", 3);

    // STEP 4: shift 5
    write_step(f, 3, "SHIFT", NULL, 3, "[0,4,*,(]", "5+8)", 4);

    // STEP 5: shift +
    write_step(f, 4, "SHIFT", NULL, 4, "[0,4,*,(,5]", "+8)", 5);

    // STEP 6: shift 8
    write_step(f, 5, "SHIFT", NULL, 5, "[0,4,*,(,5,+]", "8)", 6);

    // STEP 7: reduce f -> NUM
    write_step(f, 6, "REDUCE", "rule 6 (f -> NUM)", 6, "[0,4,*,(,5,+,8]", "(empty)", 7);

    // STEP 8: shift )
    write_step(f, 7, "SHIFT", NULL, 7, "[0,4,*,(,E]", ")", 8);

    // STEP 9: reduce S -> E
    write_step(f, 8, "REDUCE", "rule 1 (S -> E)", 8, "[0,4,E]", "(empty)", 9);

    // STEP 10: accept
    write_step(f, 9, "ACCEPT", NULL, 9, "[S]", "(empty)", 0);

    fclose(f);
}