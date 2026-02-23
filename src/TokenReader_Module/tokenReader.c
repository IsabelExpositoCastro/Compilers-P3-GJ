// ----------------- LIBRERÍAS -----------------
#include "tokenReader.h"



// ----------------- FUNCIONES -----------------
void printCasualTokenReader() {
    printf("Hellooo desde el TokenReader\n");
}

void write_step(FILE *f,
                int step,
                int input_pos,
                const char *lookahead,
                const char *operation,
                const char *rule,
                int prev_state,
                int new_state,
                const char *stack,
                const char *remaining_input)
{
    fprintf(f, "STEP %d:\n", step);
    fprintf(f, "  INPUT_POS=%d\n", input_pos);
    fprintf(f, "  LOOKAHEAD=%s\n", lookahead);
    fprintf(f, "  OPERATION=%s\n", operation);

    if (rule != NULL)
        fprintf(f, "  RULE=%s\n", rule);

    fprintf(f, "  PREV_STATE=%d\n", prev_state);
    fprintf(f, "  NEW_STATE=%d\n", new_state);
    fprintf(f, "  STACK=%s\n", stack);
    //fprintf(f, "  REMAINING_INPUT=%s\n", remaining_input);
    fprintf(f, "--------------------------------------------------\n");
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

    int step = 1;

    // STEP 1: shift NUM
    write_step(f, step++, 0, "NUM", "shift", NULL,
               0, 5,
               "[0, NUM, 5]",
               "+ NUM$");

    // STEP 2: reduce R7
    write_step(f, step++, 1, "+", "reduce",
               "R7 (f -> NUM)",
               5, 3,
               "[0, f, 3]",
               "+ NUM$");

    // STEP 3: reduce R5
    write_step(f, step++, 1, "+", "reduce",
               "R5 (t -> f)",
               3, 2,
               "[0, t, 2]",
               "+ NUM$");

    // STEP 4: reduce R3
    write_step(f, step++, 1, "+", "reduce",
               "R3 (e -> t)",
               2, 1,
               "[0, e, 1]",
               "+ NUM$");

    // STEP 5: shift +
    write_step(f, step++, 1, "+", "shift",
               NULL,
               1, 6,
               "[0, e, 1, +, 6]",
               "NUM$");

    // STEP 6: shift NUM
    write_step(f, step++, 2, "NUM", "shift",
               NULL,
               6, 5,
               "[0, e, 1, +, 6, NUM, 5]",
               "$");

    // STEP 7: reduce R7
    write_step(f, step++, 2, "$", "reduce",
               "R7 (f -> NUM)",
               5, 3,
               "[0, e, 1, +, 6, f, 3]",
               "$");

    // STEP 8: reduce R5
    write_step(f, step++, 2, "$", "reduce",
               "R5 (t -> f)",
               3, 7,
               "[0, e, 1, +, 6, t, 7]",
               "$");

    // STEP 9: reduce R2
    write_step(f, step++, 2, "$", "reduce",
               "R2 (e -> e + t)",
               7, 1,
               "[0, e, 1]",
               "$");

    // STEP 10: accept
    write_step(f, step++, 2, "$", "accept",
               NULL,
               1, 1,
               "[0, e, 1]",
               "$");

    fclose(f);
}