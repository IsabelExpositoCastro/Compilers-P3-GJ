// --------------- LIBRERÍAS ---------------
#include "language.h"
#include <ctype.h>

enum {
    LINE_BUFFER_SIZE = 256,
    TOKEN_LITERAL_LEN_NUM = 3
};

static const char* COMMENT_PREFIX = "//";
static const char* TOKEN_NUM = "NUM";
static const char* TOKEN_PLUS = "+";
static const char* TOKEN_STAR = "*";
static const char* TOKEN_LPAREN = "(";
static const char* TOKEN_RPAREN = ")";
static const char* TOKEN_DOLLAR = "$";
static const char* TOKEN_INVALID = "INVALID";
static const char* SECTION_LANGUAGE = "LANGUAGE";
static const char* SECTION_AUTOMATA = "AUTOMATA";
static const char* RULE_ARROW = "_";
static const char* RULE_ARROW_WITH_SPACES = " _ ";
static const char* TOKEN_SPLIT_DELIMS = " \t\r\n";



// --------------- HELPERS ---------------
static void strip_comment(char* line) {
    char* comment_start = strstr(line, COMMENT_PREFIX);
    if (comment_start != NULL) {
        *comment_start = '\0';
    }
}

static void trim_whitespace(char* text) {
    int len = (int)strlen(text);
    int start = 0;
    int end = len - 1;

    while (start < len && isspace((unsigned char)text[start])) {
        start++;
    }
    while (end >= start && isspace((unsigned char)text[end])) {
        text[end] = '\0';
        end--;
    }

    if (start > 0) {
        memmove(text, text + start, strlen(text + start) + 1);
    }
}

static int add_rhs_terminal(ProductionRule* rule, Terminal terminal) {
    if (rule->rhs_len >= MAX_RHS_LEN) {
        return 0;
    }
    rule->rhs[rule->rhs_len].kind = SYM_TERMINAL;
    rule->rhs[rule->rhs_len].val.term = terminal;
    rule->rhs_len++;
    return 1;
}

static int add_rhs_nonterminal(ProductionRule* rule, char nonterminal) {
    if (rule->rhs_len >= MAX_RHS_LEN) {
        return 0;
    }
    rule->rhs[rule->rhs_len].kind = SYM_NONTERMINAL;
    rule->rhs[rule->rhs_len].val.nonterm = nonterminal;
    rule->rhs_len++;
    return 1;
}

static int parse_rhs_token(ProductionRule* rule, const char* token) {
    int index = 0;

    if (strcmp(token, TOKEN_NUM) == 0) {
        return add_rhs_terminal(rule, TOK_NUM);
    }

    while (token[index] != '\0') {
        if (strncmp(&token[index], TOKEN_NUM, TOKEN_LITERAL_LEN_NUM) == 0) {
            if (!add_rhs_terminal(rule, TOK_NUM)) {
                return 0;
            }
            index += TOKEN_LITERAL_LEN_NUM;
            continue;
        }

        if (token[index] == '+') {
            if (!add_rhs_terminal(rule, TOK_PLUS)) {
                return 0;
            }
            index++;
            continue;
        }

        if (token[index] == '*') {
            if (!add_rhs_terminal(rule, TOK_STAR)) {
                return 0;
            }
            index++;
            continue;
        }

        if (token[index] == '(') {
            if (!add_rhs_terminal(rule, TOK_LPAREN)) {
                return 0;
            }
            index++;
            continue;
        }

        if (token[index] == ')') {
            if (!add_rhs_terminal(rule, TOK_RPAREN)) {
                return 0;
            }
            index++;
            continue;
        }

        if (isalpha((unsigned char)token[index])) {
            if (!add_rhs_nonterminal(rule, token[index])) {
                return 0;
            }
            index++;
            continue;
        }

        if (isspace((unsigned char)token[index])) {
            index++;
            continue;
        }

        return 0;
    }

    return 1;
}

static int parse_rule_line(char* line, Grammar* grammar) {
    char* lhs_token = strtok(line, TOKEN_SPLIT_DELIMS);
    char* arrow_token = strtok(NULL, TOKEN_SPLIT_DELIMS);
    char* rhs_token = NULL;

    if (lhs_token == NULL || arrow_token == NULL || strcmp(arrow_token, RULE_ARROW) != 0) {
        return 0;
    }

    if (grammar->num_rules >= MAX_RULES) {
        return 0;
    }

    ProductionRule* rule = &grammar->rules[grammar->num_rules];
    rule->lhs = lhs_token[0];
    rule->rhs_len = 0;

    rhs_token = strtok(NULL, TOKEN_SPLIT_DELIMS);
    while (rhs_token != NULL) {
        if (!parse_rhs_token(rule, rhs_token)) {
            return 0;
        }
        rhs_token = strtok(NULL, TOKEN_SPLIT_DELIMS);
    }

    if (rule->rhs_len == 0) {
        return 0;
    }

    if (grammar->num_rules == 0) {
        grammar->start_symbol = rule->lhs;
    }
    grammar->num_rules++;
    return 1;
}

static void parse_terminal_line(char* line, Grammar* grammar) {
    char* token = strtok(line, TOKEN_SPLIT_DELIMS);

    while (token != NULL && grammar->num_terminals < MAX_TERMINALS) {
        Terminal t = terminal_from_lexeme(token);
        if (t != TOK_INVALID) {
            grammar->terminals[grammar->num_terminals++] = t;
        }
        token = strtok(NULL, TOKEN_SPLIT_DELIMS);
    }
}

static void parse_nonterminal_line(char* line, Grammar* grammar) {
    char* token = strtok(line, TOKEN_SPLIT_DELIMS);

    while (token != NULL && grammar->num_nonterminals < MAX_NONTERMINALS) {
        if (isalpha((unsigned char)token[0])) {
            grammar->nonterminals[grammar->num_nonterminals++] = token[0];
        }
        token = strtok(NULL, TOKEN_SPLIT_DELIMS);
    }
}



// --------------- FUNCIONES ---------------
Terminal terminal_from_lexeme(const char* lexeme) {
    if (strcmp(lexeme, TOKEN_NUM) == 0) return TOK_NUM;
    if (strcmp(lexeme, TOKEN_PLUS) == 0) return TOK_PLUS;
    if (strcmp(lexeme, TOKEN_STAR) == 0) return TOK_STAR;
    if (strcmp(lexeme, TOKEN_LPAREN) == 0) return TOK_LPAREN;
    if (strcmp(lexeme, TOKEN_RPAREN) == 0) return TOK_RPAREN;
    if (strcmp(lexeme, TOKEN_DOLLAR) == 0) return TOK_DOLLAR;
    return TOK_INVALID;
}

const char* terminal_to_string(Terminal terminal) {
    switch (terminal) {
        case TOK_NUM: return TOKEN_NUM;
        case TOK_PLUS: return TOKEN_PLUS;
        case TOK_STAR: return TOKEN_STAR;
        case TOK_LPAREN: return TOKEN_LPAREN;
        case TOK_RPAREN: return TOKEN_RPAREN;
        case TOK_DOLLAR: return TOKEN_DOLLAR;
        default: return TOKEN_INVALID;
    }
}

int load_grammar_from_file(const char* filepath, Grammar* grammar) {
    FILE* file = NULL;
    char line[LINE_BUFFER_SIZE];
    int language_section_found = 0;

    if (filepath == NULL || grammar == NULL) {
        return 0;
    }

    memset(grammar, 0, sizeof(Grammar));

    file = fopen(filepath, "r");
    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        strip_comment(line);
        trim_whitespace(line);

        if (line[0] == '\0') {
            continue;
        }

        if (!language_section_found) {
            if (strcmp(line, SECTION_LANGUAGE) == 0) {
                language_section_found = 1;
            }
            continue;
        }

        if (strcmp(line, SECTION_AUTOMATA) == 0) {
            break;
        }

        if (strstr(line, RULE_ARROW_WITH_SPACES) != NULL || strchr(line, RULE_ARROW[0]) != NULL) {
            char rule_line[LINE_BUFFER_SIZE];
            strcpy(rule_line, line);
            if (!parse_rule_line(rule_line, grammar)) {
                fclose(file);
                return 0;
            }
            continue;
        }

        if (grammar->num_terminals == 0) {
            char terminals_line[LINE_BUFFER_SIZE];
            strcpy(terminals_line, line);
            parse_terminal_line(terminals_line, grammar);
            continue;
        }

        if (grammar->num_nonterminals == 0) {
            char nonterminals_line[LINE_BUFFER_SIZE];
            strcpy(nonterminals_line, line);
            parse_nonterminal_line(nonterminals_line, grammar);
            continue;
        }
    }

    fclose(file);
    return (grammar->num_rules > 0 && grammar->num_terminals > 0 && grammar->num_nonterminals > 0);
}