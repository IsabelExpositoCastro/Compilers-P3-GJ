// --------------- LIBRERÍAS ---------------
#include "parser.h"
#include <string.h>



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
    append_text(output, output_size, "[");

    if (stack->size > 0) {
        char state_text[32];
        snprintf(state_text, sizeof(state_text), "%d", stack->data[0].state);
        append_text(output, output_size, state_text);
    }

    for (index = 1; index < stack->size; index++) {
        char piece[64];
        const StackItem* item = &stack->data[index];
        char symbol_text[16];

        if (item->kind == SYM_TERMINAL) {
            snprintf(symbol_text, sizeof(symbol_text), "%s", terminal_to_string(item->sym.term));
        } else {
            snprintf(symbol_text, sizeof(symbol_text), "%c", item->sym.nonterm);
        }

        snprintf(piece, sizeof(piece), ", %s, %d", symbol_text, item->state);
        append_text(output, output_size, piece);
    }

    append_text(output, output_size, "]");
}

static void build_remaining_input_text(const TokenArray* expression, int input_pos, char* output, size_t output_size) {
    int index;
    output[0] = '\0';

    for (index = input_pos; index < expression->size; index++) {
        append_text(output, output_size, expression->tokens[index].lexeme);
    }
    append_text(output, output_size, "$");
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
        snprintf(output, output_size, "R%d", rule_number);
        return;
    }

    rule = &grammar->rules[rule_number - 1];
    snprintf(lhs, sizeof(lhs), "%c -> ", rule->lhs);
    output[0] = '\0';
    append_text(output, output_size, lhs);

    for (index = 0; index < rule->rhs_len; index++) {
        if (rule->rhs[index].kind == SYM_TERMINAL) {
            append_text(output, output_size, terminal_to_string(rule->rhs[index].val.term));
        } else {
            char nt[2] = {rule->rhs[index].val.nonterm, '\0'};
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
    char expression_text[512];

    fprintf(dbg_out, "Expression %d\n", expression_index + 1);
    build_full_expression_text(expression, expression_text, sizeof(expression_text));
    fprintf(dbg_out, "EXPR=%s\n", expression_text);
    fprintf(dbg_out, "-------------------------------------\n");

    stack_init(&stack);
    if (stack.data == NULL) {
        return 0;
    }

    memset(&start_item, 0, sizeof(StackItem));
    start_item.kind = SYM_NONTERMINAL;
    start_item.sym.nonterm = '#';
    start_item.state = 0;
    if (!stack_push(&stack, start_item)) {
        stack_free(&stack);
        return 0;
    }

    while (1) {
        int current_state = stack_top_state(&stack);
        Terminal lookahead = (input_pos < expression->size) ? expression->tokens[input_pos].category : TOK_DOLLAR;
        Action action;
        char stack_text[512];
        char remaining_text[512];
        char action_text[256];

        if (current_state < 0 || current_state >= automaton->num_states) {
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
            snprintf(action_text, sizeof(action_text), "Unexpected token at input position %d", input_pos);
            write_step(dbg_out, input_pos, "ERROR", current_state, stack_text, remaining_text, action_text);
            stack_free(&stack);
            return 0;
        }

        action = automata_get_action(automaton, current_state, lookahead);

        if (action.type == ACT_SHIFT) {
            StackItem shifted;
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
            snprintf(action_text, sizeof(action_text), "Shift from state %d to state %d", current_state, action.value);
            write_step(dbg_out, input_pos, "SHIFT", current_state, stack_text, remaining_text, action_text);

            memset(&shifted, 0, sizeof(StackItem));
            shifted.kind = SYM_TERMINAL;
            shifted.sym.term = lookahead;
            shifted.state = action.value;
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
            int goto_state;
            StackItem reduced;
            char rule_text[128];

            if (rule_number <= 0 || rule_number > grammar->num_rules) {
                stack_free(&stack);
                return 0;
            }

            rule = &grammar->rules[rule_number - 1];
            build_rule_text(grammar, rule_number, rule_text, sizeof(rule_text));
            build_stack_text(&stack, stack_text, sizeof(stack_text));
            build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
            snprintf(action_text, sizeof(action_text), "Reduce by R%d (%s)", rule_number, rule_text);
            write_step(dbg_out, input_pos, "REDUCE", current_state, stack_text, remaining_text, action_text);

            for (pop_count = 0; pop_count < rule->rhs_len; pop_count++) {
                if (!stack_pop(&stack, NULL)) {
                    stack_free(&stack);
                    return 0;
                }
            }

            current_state = stack_top_state(&stack);
            if (current_state < 0 || current_state >= automaton->num_states) {
                stack_free(&stack);
                return 0;
            }

            goto_state = automata_get_goto(automaton, current_state, rule->lhs);
            if (goto_state < 0) {
                stack_free(&stack);
                return 0;
            }

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
            write_step(dbg_out, input_pos, "ACCEPT", current_state, stack_text, remaining_text, action_text);
            stack_free(&stack);
            return 1;
        }

        build_stack_text(&stack, stack_text, sizeof(stack_text));
        build_remaining_input_text(expression, input_pos, remaining_text, sizeof(remaining_text));
        snprintf(action_text, sizeof(action_text), "No action for state=%d lookahead=%s", current_state, terminal_to_string(lookahead));
        write_step(dbg_out, input_pos, "ERROR", current_state, stack_text, remaining_text, action_text);
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
        fprintf(dbg_out, "-------------------------------------\n\n");
    }

    return all_ok;
}