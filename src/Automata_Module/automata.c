// --------------- LIBRERÍAS ---------------
#include "automata.h"
#include <ctype.h>
#include <string.h>



// --------------- HELPERS ---------------
enum {
    TERM_NUM = 0,
    TERM_PLUS,
    TERM_STAR,
    TERM_LPAREN,
    TERM_RPAREN,
    TERM_DOLLAR,
    TERM_COUNT
};

#define MAX_AUT_STATES 128
#define MAX_ACTION_COLS 16

typedef enum {
    MODE_NONE = 0,
    MODE_ACTION,
    MODE_GOTO
} ParseMode;

static int terminal_to_col(Terminal terminal) {
    switch (terminal) {
        case TOK_NUM: return TERM_NUM;
        case TOK_PLUS: return TERM_PLUS;
        case TOK_STAR: return TERM_STAR;
        case TOK_LPAREN: return TERM_LPAREN;
        case TOK_RPAREN: return TERM_RPAREN;
        case TOK_DOLLAR: return TERM_DOLLAR;
        default: return -1;
    }
}

static int action_header_to_col(const char* token) {
    if (strcmp(token, "NUM") == 0) return TERM_NUM;
    if (strcmp(token, "+") == 0) return TERM_PLUS;
    if (strcmp(token, "*") == 0) return TERM_STAR;
    if (strcmp(token, "(") == 0) return TERM_LPAREN;
    if (strcmp(token, ")") == 0) return TERM_RPAREN;
    if (strcmp(token, "$") == 0) return TERM_DOLLAR;
    return -1;
}

static int nonterminal_to_col(const Automaton* automaton, char nonterminal) {
    int index;
    for (index = 0; index < automaton->num_goto_nonterminals; index++) {
        if (automaton->goto_nonterminals[index] == nonterminal) {
            return index;
        }
    }
    return -1;
}

static int alloc_tables(Automaton* automaton, int states, int goto_cols) {
    int i;
    automaton->num_states = states;
    automaton->start_state = 0;
    automaton->num_goto_nonterminals = goto_cols;

    automaton->action_table = (Action**)malloc(sizeof(Action*) * states);
    automaton->goto_table = (int**)malloc(sizeof(int*) * states);
    automaton->goto_nonterminals = (char*)malloc(sizeof(char) * goto_cols);

    if (automaton->action_table == NULL || automaton->goto_table == NULL || automaton->goto_nonterminals == NULL) {
        return 0;
    }

    for (i = 0; i < states; i++) {
        int j;
        automaton->action_table[i] = (Action*)malloc(sizeof(Action) * TERM_COUNT);
        automaton->goto_table[i] = (int*)malloc(sizeof(int) * goto_cols);
        if (automaton->action_table[i] == NULL || automaton->goto_table[i] == NULL) {
            return 0;
        }

        for (j = 0; j < TERM_COUNT; j++) {
            automaton->action_table[i][j].type = ACT_ERROR;
            automaton->action_table[i][j].value = -1;
        }
        for (j = 0; j < goto_cols; j++) {
            automaton->goto_table[i][j] = -1;
        }
    }
    return 1;
}

static void strip_comment(char* line) {
    char* comment = strstr(line, "//");
    if (comment != NULL) {
        *comment = '\0';
    }
}

static void trim_line(char* line) {
    int start = 0;
    int end = (int)strlen(line) - 1;

    while (line[start] != '\0' && isspace((unsigned char)line[start])) start++;
    while (end >= start && isspace((unsigned char)line[end])) {
        line[end] = '\0';
        end--;
    }

    if (start > 0) {
        memmove(line, line + start, strlen(line + start) + 1);
    }
}

static int is_empty_line(const char* line) {
    return line == NULL || line[0] == '\0';
}

static int parse_action_cell(const char* token, Action* out_action) {
    if (strcmp(token, "-") == 0) {
        out_action->type = ACT_ERROR;
        out_action->value = -1;
        return 1;
    }

    if (strcmp(token, "acc") == 0 || strcmp(token, "accept") == 0) {
        out_action->type = ACT_ACCEPT;
        out_action->value = 0;
        return 1;
    }

    if (token[0] == 's' || token[0] == 'S') {
        out_action->type = ACT_SHIFT;
        out_action->value = atoi(token + 1);
        return 1;
    }

    if (token[0] == 'r' || token[0] == 'R') {
        out_action->type = ACT_REDUCE;
        out_action->value = atoi(token + 1);
        return 1;
    }

    return 0;
}




// --------------- FUNCIONES ---------------
int automata_load_from_file(const char* language_path, Automaton* automaton) {
    FILE* file;
    char line[512];
    ParseMode mode = MODE_NONE;
    int automata_section_found = 0;
    int num_states = -1;
    int start_state = 0;
    int metadata_lines_read = 0;

    int action_header_ready = 0;
    int goto_header_ready = 0;
    int action_col_map[MAX_ACTION_COLS];
    int action_cols = 0;
    char goto_columns[MAX_NONTERMINALS];
    int goto_cols = 0;

    Action action_tmp[MAX_AUT_STATES][TERM_COUNT];
    int goto_tmp[MAX_AUT_STATES][MAX_NONTERMINALS];
    int i, j;

    if (language_path == NULL || automaton == NULL) {
        return 0;
    }

    memset(automaton, 0, sizeof(Automaton));
    for (i = 0; i < MAX_AUT_STATES; i++) {
        for (j = 0; j < TERM_COUNT; j++) {
            action_tmp[i][j].type = ACT_ERROR;
            action_tmp[i][j].value = -1;
        }
        for (j = 0; j < MAX_NONTERMINALS; j++) {
            goto_tmp[i][j] = -1;
        }
    }

    file = fopen(language_path, "r");
    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char* tokens[64];
        int token_count = 0;
        char* tok;

        strip_comment(line);
        trim_line(line);
        if (is_empty_line(line)) {
            continue;
        }

        if (!automata_section_found) {
            if (strcmp(line, "AUTOMATA") == 0) {
                automata_section_found = 1;
            }
            continue;
        }

        if (metadata_lines_read < 3) {
            int value = atoi(line);
            if (metadata_lines_read == 0) num_states = value;
            if (metadata_lines_read == 1) start_state = value;
            metadata_lines_read++;
            continue;
        }

        if (strcmp(line, "ACTION") == 0) {
            mode = MODE_ACTION;
            action_header_ready = 0;
            continue;
        }
        if (strcmp(line, "GOTO") == 0) {
            mode = MODE_GOTO;
            goto_header_ready = 0;
            continue;
        }

        tok = strtok(line, " \t\r\n");
        while (tok != NULL && token_count < 64) {
            tokens[token_count++] = tok;
            tok = strtok(NULL, " \t\r\n");
        }
        if (token_count == 0) continue;

        if (mode == MODE_ACTION && !action_header_ready) {
            int k;
            if (token_count < 2 || strcmp(tokens[0], "STATE") != 0) {
                fclose(file);
                return 0;
            }
            action_cols = 0;
            for (k = 1; k < token_count && action_cols < MAX_ACTION_COLS; k++) {
                int col = action_header_to_col(tokens[k]);
                if (col >= 0) {
                    action_col_map[action_cols++] = col;
                }
            }
            action_header_ready = (action_cols > 0);
            continue;
        }

        if (mode == MODE_GOTO && !goto_header_ready) {
            int k;
            if (token_count < 2 || strcmp(tokens[0], "STATE") != 0) {
                fclose(file);
                return 0;
            }
            goto_cols = 0;
            for (k = 1; k < token_count && goto_cols < MAX_NONTERMINALS; k++) {
                if (strlen(tokens[k]) == 1 && isalpha((unsigned char)tokens[k][0])) {
                    goto_columns[goto_cols++] = tokens[k][0];
                }
            }
            goto_header_ready = (goto_cols > 0);
            continue;
        }

        if (mode == MODE_ACTION && action_header_ready) {
            int state = atoi(tokens[0]);
            int k;
            if (state < 0 || state >= MAX_AUT_STATES) {
                fclose(file);
                return 0;
            }
            for (k = 1; k < token_count && (k - 1) < action_cols; k++) {
                Action parsed;
                if (!parse_action_cell(tokens[k], &parsed)) {
                    fclose(file);
                    return 0;
                }
                action_tmp[state][action_col_map[k - 1]] = parsed;
            }
            continue;
        }

        if (mode == MODE_GOTO && goto_header_ready) {
            int state = atoi(tokens[0]);
            int k;
            if (state < 0 || state >= MAX_AUT_STATES) {
                fclose(file);
                return 0;
            }
            for (k = 1; k < token_count && (k - 1) < goto_cols; k++) {
                if (strcmp(tokens[k], "-") == 0) {
                    goto_tmp[state][k - 1] = -1;
                } else {
                    goto_tmp[state][k - 1] = atoi(tokens[k]);
                }
            }
        }
    }

    fclose(file);

    if (!automata_section_found || num_states <= 0 || num_states > MAX_AUT_STATES || goto_cols <= 0) {
        return 0;
    }

    if (!alloc_tables(automaton, num_states, goto_cols)) {
        automata_free(automaton);
        return 0;
    }

    automaton->start_state = start_state;
    for (i = 0; i < goto_cols; i++) {
        automaton->goto_nonterminals[i] = goto_columns[i];
    }

    for (i = 0; i < num_states; i++) {
        for (j = 0; j < TERM_COUNT; j++) {
            automaton->action_table[i][j] = action_tmp[i][j];
        }
        for (j = 0; j < goto_cols; j++) {
            automaton->goto_table[i][j] = goto_tmp[i][j];
        }
    }

    return 1;
}

void automata_free(Automaton* automaton) {
    int i;
    if (automaton == NULL) return;

    if (automaton->action_table != NULL) {
        for (i = 0; i < automaton->num_states; i++) {
            free(automaton->action_table[i]);
        }
        free(automaton->action_table);
    }

    if (automaton->goto_table != NULL) {
        for (i = 0; i < automaton->num_states; i++) {
            free(automaton->goto_table[i]);
        }
        free(automaton->goto_table);
    }

    free(automaton->goto_nonterminals);
    memset(automaton, 0, sizeof(Automaton));
}

Action automata_get_action(const Automaton* automaton, int state, Terminal terminal) {
    Action error_action = {ACT_ERROR, -1};
    int term_col;
    if (automaton == NULL || state < 0 || state >= automaton->num_states) {
        return error_action;
    }

    term_col = terminal_to_col(terminal);
    if (term_col < 0) {
        return error_action;
    }

    return automaton->action_table[state][term_col];
}

int automata_get_goto(const Automaton* automaton, int state, char nonterminal) {
    int col;
    if (automaton == NULL || state < 0 || state >= automaton->num_states) {
        return -1;
    }

    col = nonterminal_to_col(automaton, nonterminal);
    if (col < 0) {
        return -1;
    }

    return automaton->goto_table[state][col];
}