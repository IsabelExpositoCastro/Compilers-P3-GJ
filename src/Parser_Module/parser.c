// --------------- LIBRERÍAS ---------------
#include "parser.h"
#include <string.h>

enum {
    STACK_TEXT_SIZE = 512,
    REMAINING_TEXT_SIZE = 512,
    ACTION_TEXT_SIZE = 512,
    EXPRESSION_TEXT_SIZE = 512,
    RULE_TEXT_SIZE = 128,
    POPPED_TEXT_SIZE = 320,
    ITEM_TEXT_SIZE = 64,
    PIECE_TEXT_SIZE = 64,
    STATE_TEXT_SIZE = 32,
    SYMBOL_TEXT_SIZE = 16,
    START_STATE = 0,
    MIN_STACK_SIZE = 0,
    RULE_INDEX_OFFSET = 1,
    INVALID_STATE = -1,
    SINGLE_CHAR_TEXT_SIZE = 2
};

static const char* STACK_OPEN = "[";
static const char* STACK_CLOSE = "]";
static const char* STACK_SEP = ", ";
static const char* EMPTY_STRING = "";
static const char* INPUT_END_MARKER = "$";
static const char* RULE_ARROW_TEXT = " -> ";
static const char* RULE_FALLBACK_FMT = "R%d";
static const char* EXPR_HEADER_FMT = "Expression %d\n";
static const char* EXPR_LINE_FMT = "EXPR=%s\n";
static const char* BLOCK_SEPARATOR = "-------------------------------------\n";
static const char* BLOCK_SEPARATOR_EXTRA = "-------------------------------------\n\n";
static const char* OP_ERROR = "E";
static const char* OP_SHIFT = "S";
static const char* OP_REDUCE = "R";
static const char* OP_ACCEPT = "A";
static const char START_SYMBOL_SENTINEL = '#';



// --------------- HELPERS ---------------
static void append_text(char* destination, size_t max_size, const char* text) {
    size_t current_len = strlen(destination);
    size_t remaining = (current_len < max_size) ? (max_size - current_len - 1) : 0;
    if (remaining > 0) {
        strncat(destination, text, remaining);
    }
}

static void build_stack_text(const ParserStack* stack, char* output, size_t output_size) {
    int index;
    output[0] = '\0';
    append_text(output, output_size, STACK_OPEN);

    if (stack->size > MIN_STACK_SIZE) {
        char state_text[STATE_TEXT_SIZE];
        snprintf(state_text, sizeof(state_text), "%d", stack->data[0].state);
        append_text(output, output_size, state_text);
    }

    for (index = 1; index < stack->size; index++) {
        char piece[PIECE_TEXT_SIZE];
        const StackItem* item = &stack->data[index];
        char symbol_text[SYMBOL_TEXT_SIZE];

        if (item->kind == SYM_TERMINAL) {
            snprintf(symbol_text, sizeof(symbol_text), "%s", terminal_to_string(item->sym.term));
        } else {
            snprintf(symbol_text, sizeof(symbol_text), "%c", item->sym.nonterm);
        }

        snprintf(piece, sizeof(piece), "%s%s, %d", STACK_SEP, symbol_text, item->state);
        append_text(output, output_size, piece);
    }

    append_text(output, output_size, STACK_CLOSE);
}

static void build_stack_item_text(const StackItem* item, char* output, size_t output_size) {
    if (item->kind == SYM_TERMINAL) {
        snprintf(output, output_size, "[%s, %d]", terminal_to_string(item->sym.term), item->state);
    } else {
        snprintf(output, output_size, "[%c, %d]", item->sym.nonterm, item->state);
    }
}

static void build_remaining_input_text(const TokenArray* expression, int input_pos, char* output, size_t output_size) {
    int index;
    output[0] = '\0';

    for (index = input_pos; index < expression->size; index++) {
        append_text(output, output_size, expression->tokens[index].lexeme);
    }
    append_text(output, output_size, INPUT_END_MARKER);
}

static void build_full_expression_text(const TokenArray* expression, char* output, size_t output_size) {
    int index;
    output[0] = '\0';
    for (index = 0; index < expression->size; index++) {
        append_text(output, output_size, expression->tokens[index].lexeme);
    }
}

static void build_rule_text(const Grammar* grammar, int rule_number, char* output, size_t output_size) {
    int index;
    const ProductionRule* rule;
    char lhs[8];

    if (rule_number <= 0 || rule_number > grammar->num_rules) {
        snprintf(output, output_size, RULE_FALLBACK_FMT, rule_number);
        return;
    }

    rule = &grammar->rules[rule_number - RULE_INDEX_OFFSET];
    snprintf(lhs, sizeof(lhs), "%c%s", rule->lhs, RULE_ARROW_TEXT);
    output[0] = '\0';
    append_text(output, output_size, lhs);

    for (index = 0; index < rule->rhs_len; index++) {
        if (rule->rhs[index].kind == SYM_TERMINAL) {
            append_text(output, output_size, terminal_to_string(rule->rhs[index].val.term));
        } else {
            char nt[SINGLE_CHAR_TEXT_SIZE] = {rule->rhs[index].val.nonterm, '\0'};
            append_text(output, output_size, nt);
        }
        if (index + 1 < rule->rhs_len) {
            append_text(output, output_size, " ");
        }
    }
}

static int run_expression(const Grammar* grammar, const Automaton* automaton, const TokenArray* expression, FILE* dbg_out, int expression_index) {
    ParserStack stack;
    StackItem start_item;
    int input_pos = 0;
    char expression_text[EXPRESSION_TEXT_SIZE];

    fprintf(dbg_out, EXPR_HEADER_FMT, expression_index + RULE_INDEX_OFFSET);
    build_full_expression_text(expression, expression_text, sizeof(expression_text));
    fprintf(dbg_out, EXPR_LINE_FMT, expression_text);
    fprintf(dbg_out, "%s", BLOCK_SEPARATOR);

    stack_init(&stack);
    if (stack.data == NULL) {
        return 0;
    }

    memset(&start_item, 0, sizeof(StackItem));
    start_item.kind = SYM_NONTERMINAL;
    start_item.sym.nonterm = START_SYMBOL_SENTINEL;
    start_item.state = START_STATE;
    if (!stack_push(&stack, start_item)) {
        stack_free(&stack);
        return 0;
    }

    while (1) {
        int current_state = stack_top_state(&stack);
        Terminal lookahead = (input_pos < expression->size) ? expression->tokens[input_pos].category : TOK_DOLLAR;
        Action action;
        char stack_text[STACK_TEXT_SIZE];
        char remaining_text[REMAINING_TEXT_SIZE];
        char action_text[ACTION_TEXT_SIZE];

        if (current_state < 0 || current_state >= automaton->num_states) {
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
            snprintf(action_text, sizeof(action_text), "Unexpected token at input position %d", input_pos);
            write_step(dbg_out, input_pos, OP_ERROR, current_state, stack_text, remaining_text, action_text);
            stack_free(&stack);
            return 0;
        }

        action = automata_get_action(automaton, current_state, lookahead);

        if (action.type == ACT_SHIFT) {
            StackItem shifted;
            char pushed_text[ITEM_TEXT_SIZE];
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));

            memset(&shifted, 0, sizeof(StackItem));
            shifted.kind = SYM_TERMINAL;
            shifted.sym.term = lookahead;
            shifted.state = action.value;
            build_stack_item_text(&shifted, pushed_text, sizeof(pushed_text));

            snprintf(action_text,
                     sizeof(action_text),
                     "Shift from state %d to state %d | PUSH %s",
                     current_state,
                     action.value,
                     pushed_text);
            write_step(dbg_out, input_pos, OP_SHIFT, current_state, stack_text, remaining_text, action_text);

            if (!stack_push(&stack, shifted)) {
                stack_free(&stack);
                return 0;
            }
            input_pos++;
            continue;
        }

        if (action.type == ACT_REDUCE) {
            int rule_number = action.value;
            const ProductionRule* rule;
            int pop_count;
            int goto_from_state;
            int goto_state;
            StackItem reduced;
            char rule_text[RULE_TEXT_SIZE];
            char popped_text[POPPED_TEXT_SIZE];

            if (rule_number <= 0 || rule_number > grammar->num_rules) {
                stack_free(&stack);
                return 0;
            }

            rule = &grammar->rules[rule_number - RULE_INDEX_OFFSET];
            build_rule_text(grammar, rule_number, rule_text, sizeof(rule_text));
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
            popped_text[0] = EMPTY_STRING[0];

            for (pop_count = 0; pop_count < rule->rhs_len; pop_count++) {
                StackItem popped;
                char item_text[ITEM_TEXT_SIZE];

                if (!stack_pop(&stack, &popped)) {
                    stack_free(&stack);
                    return 0;
                }

                build_stack_item_text(&popped, item_text, sizeof(item_text));
                if (pop_count > 0) {
                    append_text(popped_text, sizeof(popped_text), ", ");
                }
                append_text(popped_text, sizeof(popped_text), item_text);
            }

            goto_from_state = stack_top_state(&stack);
            if (goto_from_state < 0 || goto_from_state >= automaton->num_states) {
                stack_free(&stack);
                return 0;
            }

            goto_state = automata_get_goto(automaton, goto_from_state, rule->lhs);
            if (goto_state < 0) {
                stack_free(&stack);
                return 0;
            }

            snprintf(action_text,
                     sizeof(action_text),
                     "Reduce by R%d (%s) | POP {%s} | PUSH [%c, %d] | GOTO [%d, %c]=%d",
                     rule_number,
                     rule_text,
                     popped_text,
                     rule->lhs,
                     goto_state,
                     goto_from_state,
                     rule->lhs,
                     goto_state);
            write_step(dbg_out, input_pos, OP_REDUCE, current_state, stack_text, remaining_text, action_text);

            memset(&reduced, 0, sizeof(StackItem));
            reduced.kind = SYM_NONTERMINAL;
            reduced.sym.nonterm = rule->lhs;
            reduced.state = goto_state;
            if (!stack_push(&stack, reduced)) {
                stack_free(&stack);
                return 0;
            }
            continue;
        }

        if (action.type == ACT_ACCEPT) {
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
            snprintf(action_text, sizeof(action_text), "Input accepted");
            write_step(dbg_out, input_pos, OP_ACCEPT, current_state, stack_text, remaining_text, action_text);
            stack_free(&stack);
            return 1;
        }

        build_stack_text(&stack, stack_text, sizeof(stack_text));
        build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
        snprintf(action_text, sizeof(action_text), "No action for state=%d lookahead=%s", current_state, terminal_to_string(lookahead));
        write_step(dbg_out, input_pos, OP_ERROR, current_state, stack_text, remaining_text, action_text);
        stack_free(&stack);
        return 0;
    }
}




// --------------- FUNCIONES ---------------
int run_shift_reduce_parser(const Grammar* grammar, const Automaton* automaton, const TokenStream* stream, FILE* dbg_out) {
    int index;
    int all_ok = 1;
    if (grammar == NULL || automaton == NULL || stream == NULL || dbg_out == NULL) {
        return 0;
    }

    for (index = 0; index < stream->size; index++) {
        int accepted = run_expression(grammar, automaton, &stream->expressions[index], dbg_out, index);
        if (!accepted) {
            all_ok = 0;
        }
        fprintf(dbg_out, "%s", BLOCK_SEPARATOR_EXTRA);
    }

    return all_ok;
}