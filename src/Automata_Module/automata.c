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

enum {
    LINE_BUFFER_SIZE = 512,
    TOKEN_ARRAY_SIZE = 64,
    METADATA_LINES_COUNT = 3,
    STATE_HEADER_MIN_COLS = 2,
    SECTION_START_INDEX = 1,
    TOKEN_LITERAL_LEN_SINGLE = 1,
    START_STATE_DEFAULT = 0,
    INVALID_VALUE = -1
};

static const char* COMMENT_PREFIX = "//";
static const char* TOKEN_NUM = "NUM";
static const char* TOKEN_PLUS = "+";
static const char* TOKEN_STAR = "*";
static const char* TOKEN_LPAREN = "(";
static const char* TOKEN_RPAREN = ")";
static const char* TOKEN_DOLLAR = "$";
static const char* TOKEN_STATE = "STATE";
static const char* TOKEN_DASH = "-";
static const char* TOKEN_ACC = "acc";
static const char* TOKEN_ACCEPT = "accept";
static const char* SECTION_AUTOMATA = "AUTOMATA";
static const char* SECTION_ACTION = "ACTION";
static const char* SECTION_GOTO = "GOTO";
static const char* TOKEN_SPLIT_DELIMS = " \t\r\n";

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
    if (strcmp(token, TOKEN_NUM) == 0) return TERM_NUM;
    if (strcmp(token, TOKEN_PLUS) == 0) return TERM_PLUS;
    if (strcmp(token, TOKEN_STAR) == 0) return TERM_STAR;
    if (strcmp(token, TOKEN_LPAREN) == 0) return TERM_LPAREN;
    if (strcmp(token, TOKEN_RPAREN) == 0) return TERM_RPAREN;
    if (strcmp(token, TOKEN_DOLLAR) == 0) return TERM_DOLLAR;
    return INVALID_VALUE;
}

static int nonterminal_to_col(const Automaton* automaton, char nonterminal) {
    int index;
    for (index = 0; index < automaton->num_goto_nonterminals; index++) {
        if (automaton->goto_nonterminals[index] == nonterminal) {
            return index;
        }
    }
    return INVALID_VALUE;
}

static int alloc_tables(Automaton* automaton, int states, int goto_cols) {
    int i;
    automaton->num_states = states;
    automaton->start_state = START_STATE_DEFAULT;
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
            automaton->action_table[i][j].value = INVALID_VALUE;
        }
        for (j = 0; j < goto_cols; j++) {
            automaton->goto_table[i][j] = INVALID_VALUE;
        }
    }
    return 1;
}

static void strip_comment(char* line) {
    char* comment = strstr(line, COMMENT_PREFIX);
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
    if (strcmp(token, TOKEN_DASH) == 0) {
        out_action->type = ACT_ERROR;
        out_action->value = INVALID_VALUE;
        return 1;
    }

    if (strcmp(token, TOKEN_ACC) == 0 || strcmp(token, TOKEN_ACCEPT) == 0) {
        out_action->type = ACT_ACCEPT;
        out_action->value = START_STATE_DEFAULT;
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
    char line[LINE_BUFFER_SIZE];
    ParseMode mode = MODE_NONE;
    int automata_section_found = 0;
    int num_states = INVALID_VALUE;
    int start_state = START_STATE_DEFAULT;
    int metadata_lines_read = START_STATE_DEFAULT;

    int action_header_ready = 0;
    int goto_header_ready = 0;
    int action_col_map[MAX_ACTION_COLS];
    int action_cols = START_STATE_DEFAULT;
    char goto_columns[MAX_NONTERMINALS];
    int goto_cols = START_STATE_DEFAULT;

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
            action_tmp[i][j].value = INVALID_VALUE;
        }
        for (j = 0; j < MAX_NONTERMINALS; j++) {
            goto_tmp[i][j] = INVALID_VALUE;
        }
    }

    file = fopen(language_path, "r");
    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        char* tokens[TOKEN_ARRAY_SIZE];
        int token_count = START_STATE_DEFAULT;
        char* tok;

        strip_comment(line);
        trim_line(line);
        if (is_empty_line(line)) {
            continue;
        }

        if (!automata_section_found) {
            if (strcmp(line, SECTION_AUTOMATA) == 0) {
                automata_section_found = 1;
            }
            continue;
        }

        if (metadata_lines_read < METADATA_LINES_COUNT) {
            int value = atoi(line);
            if (metadata_lines_read == START_STATE_DEFAULT) num_states = value;
            if (metadata_lines_read == SECTION_START_INDEX) start_state = value;
            metadata_lines_read++;
            continue;
        }

        if (strcmp(line, SECTION_ACTION) == 0) {
            mode = MODE_ACTION;
            action_header_ready = START_STATE_DEFAULT;
            continue;
        }
        if (strcmp(line, SECTION_GOTO) == 0) {
            mode = MODE_GOTO;
            goto_header_ready = START_STATE_DEFAULT;
            continue;
        }

        tok = strtok(line, TOKEN_SPLIT_DELIMS);
        while (tok != NULL && token_count < TOKEN_ARRAY_SIZE) {
            tokens[token_count++] = tok;
            tok = strtok(NULL, TOKEN_SPLIT_DELIMS);
        }
        if (token_count == START_STATE_DEFAULT) continue;

        if (mode == MODE_ACTION && !action_header_ready) {
            int k;
            if (token_count < STATE_HEADER_MIN_COLS || strcmp(tokens[0], TOKEN_STATE) != 0) {
                fclose(file);
                return 0;
            }
            action_cols = START_STATE_DEFAULT;
            for (k = SECTION_START_INDEX; k < token_count && action_cols < MAX_ACTION_COLS; k++) {
                int col = action_header_to_col(tokens[k]);
                if (col >= START_STATE_DEFAULT) {
                    action_col_map[action_cols++] = col;
                }
            }
            action_header_ready = (action_cols > START_STATE_DEFAULT);
            continue;
        }

        if (mode == MODE_GOTO && !goto_header_ready) {
            int k;
            if (token_count < STATE_HEADER_MIN_COLS || strcmp(tokens[0], TOKEN_STATE) != 0) {
                fclose(file);
                return 0;
            }
            goto_cols = START_STATE_DEFAULT;
            for (k = SECTION_START_INDEX; k < token_count && goto_cols < MAX_NONTERMINALS; k++) {
                if (strlen(tokens[k]) == TOKEN_LITERAL_LEN_SINGLE && isalpha((unsigned char)tokens[k][0])) {
                    goto_columns[goto_cols++] = tokens[k][0];
                }
            }
            goto_header_ready = (goto_cols > START_STATE_DEFAULT);
            continue;
        }

        if (mode == MODE_ACTION && action_header_ready) {
            int state = atoi(tokens[0]);
            int k;
            if (state < START_STATE_DEFAULT || state >= MAX_AUT_STATES) {
                fclose(file);
                return 0;
            }
            for (k = SECTION_START_INDEX; k < token_count && (k - SECTION_START_INDEX) < action_cols; k++) {
                Action parsed;
                if (!parse_action_cell(tokens[k], &parsed)) {
                    fclose(file);
                    return 0;
                }
                action_tmp[state][action_col_map[k - SECTION_START_INDEX]] = parsed;
            }
            continue;
        }

        if (mode == MODE_GOTO && goto_header_ready) {
            int state = atoi(tokens[0]);
            int k;
            if (state < START_STATE_DEFAULT || state >= MAX_AUT_STATES) {
                fclose(file);
                return 0;
            }
            for (k = SECTION_START_INDEX; k < token_count && (k - SECTION_START_INDEX) < goto_cols; k++) {
                if (strcmp(tokens[k], TOKEN_DASH) == 0) {
                    goto_tmp[state][k - SECTION_START_INDEX] = INVALID_VALUE;
                } else {
                    goto_tmp[state][k - SECTION_START_INDEX] = atoi(tokens[k]);
                }
            }
        }
    }

    fclose(file);

    if (!automata_section_found || num_states <= START_STATE_DEFAULT || num_states > MAX_AUT_STATES || goto_cols <= START_STATE_DEFAULT) {
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
    Action error_action = {ACT_ERROR, INVALID_VALUE};
    int term_col;
    if (automaton == NULL || state < START_STATE_DEFAULT || state >= automaton->num_states) {
        return error_action;
    }

    term_col = terminal_to_col(terminal);
    if (term_col < START_STATE_DEFAULT) {
        return error_action;
    }

    return automaton->action_table[state][term_col];
}

int automata_get_goto(const Automaton* automaton, int state, char nonterminal) {
    int col;
    if (automaton == NULL || state < START_STATE_DEFAULT || state >= automaton->num_states) {
        return INVALID_VALUE;
    }

    col = nonterminal_to_col(automaton, nonterminal);
    if (col < START_STATE_DEFAULT) {
        return INVALID_VALUE;
    }

    return automaton->goto_table[state][col];
}