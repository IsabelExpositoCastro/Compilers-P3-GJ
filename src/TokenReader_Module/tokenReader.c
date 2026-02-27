// ----------------- LIBRERÍAS -----------------
#include "tokenReader.h"
#include <ctype.h>



// ----------------- HELPERS -----------------
static void trim_text(char* text) {
    int start = 0;
    int end = (int)strlen(text) - 1;

    while (text[start] != '\0' && isspace((unsigned char)text[start])) start++;
    while (end >= start && isspace((unsigned char)text[end])) {
        text[end] = '\0';
        end--;
    }

    if (start > 0) {
        memmove(text, text + start, strlen(text + start) + 1);
    }
}

static Terminal category_to_terminal(const char* lexeme, const char* category) {
    if (strcmp(category, "CAT_NUMBER") == 0) {
        return TOK_NUM;
    }
    if (strcmp(category, "CAT_OPERATOR") == 0) {
        if (strcmp(lexeme, "+") == 0) return TOK_PLUS;
        if (strcmp(lexeme, "*") == 0) return TOK_STAR;
    }
    if (strcmp(category, "CAT_SPECIALCHAR") == 0) {
        if (strcmp(lexeme, "(") == 0) return TOK_LPAREN;
        if (strcmp(lexeme, ")") == 0) return TOK_RPAREN;
    }
    return TOK_INVALID;
}

static void token_array_init(TokenArray* array) {
    array->size = 0;
    array->capacity = 8;
    array->tokens = (Token*)malloc(sizeof(Token) * array->capacity);
}

static void token_array_free(TokenArray* array) {
    free(array->tokens);
    array->tokens = NULL;
    array->size = 0;
    array->capacity = 0;
}

static int token_array_push(TokenArray* array, Token token) {
    Token* resized = NULL;
    if (array->size >= array->capacity) {
        array->capacity *= 2;
        resized = (Token*)realloc(array->tokens, sizeof(Token) * array->capacity);
        if (resized == NULL) return 0;
        array->tokens = resized;
    }
    array->tokens[array->size++] = token;
    return 1;
}

static void token_stream_init(TokenStream* stream) {
    stream->size = 0;
    stream->capacity = 4;
    stream->expressions = (TokenArray*)malloc(sizeof(TokenArray) * stream->capacity);
}

static int token_stream_push(TokenStream* stream, TokenArray expression) {
    TokenArray* resized = NULL;
    if (stream->size >= stream->capacity) {
        stream->capacity *= 2;
        resized = (TokenArray*)realloc(stream->expressions, sizeof(TokenArray) * stream->capacity);
        if (resized == NULL) return 0;
        stream->expressions = resized;
    }
    stream->expressions[stream->size++] = expression;
    return 1;
}

static int parse_expression_line(const char* line, TokenArray* out_expression) {
    const char* cursor = line;
    token_array_init(out_expression);

    while ((cursor = strchr(cursor, '<')) != NULL) {
        const char* end = strchr(cursor, '>');
        char tuple[128];
        char lexeme[64];
        char category[64];
        char* comma = NULL;
        Token token;

        if (end == NULL || (size_t)(end - cursor - 1) >= sizeof(tuple)) {
            token_array_free(out_expression);
            return 0;
        }

        strncpy(tuple, cursor + 1, (size_t)(end - cursor - 1));
        tuple[end - cursor - 1] = '\0';

        comma = strchr(tuple, ',');
        if (comma == NULL) {
            token_array_free(out_expression);
            return 0;
        }

        *comma = '\0';
        strcpy(lexeme, tuple);
        strcpy(category, comma + 1);
        trim_text(lexeme);
        trim_text(category);

        memset(&token, 0, sizeof(Token));
        strncpy(token.lexeme, lexeme, MAX_LEXEME_LEN - 1);
        token.lexeme[MAX_LEXEME_LEN - 1] = '\0';
        token.category = category_to_terminal(lexeme, category);

        if (!token_array_push(out_expression, token)) {
            token_array_free(out_expression);
            return 0;
        }

        cursor = end + 1;
    }

    return out_expression->size > 0;
}


// ----------------- FUNCIONES -----------------
int load_token_stream_from_file(const char* input_path, TokenStream* stream) {
    FILE* file = NULL;
    char line[512];

    if (input_path == NULL || stream == NULL) {
        return 0;
    }

    token_stream_init(stream);

    file = fopen(input_path, "r");
    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        TokenArray expression;
        if (strchr(line, '<') == NULL) {
            continue;
        }

        if (parse_expression_line(line, &expression)) {
            if (!token_stream_push(stream, expression)) {
                token_array_free(&expression);
                fclose(file);
                free_token_stream(stream);
                return 0;
            }
        }
    }

    fclose(file);
    return stream->size > 0;
}

void free_token_stream(TokenStream* stream) {
    int index;
    if (stream == NULL || stream->expressions == NULL) return;

    for (index = 0; index < stream->size; index++) {
        token_array_free(&stream->expressions[index]);
    }

    free(stream->expressions);
    stream->expressions = NULL;
    stream->size = 0;
    stream->capacity = 0;
}

void write_step(FILE *f,
                int input_pos,
                const char *operation,
                int state,
                const char *stack,
                const char *remaining_input,
                const char *action)
{
    fprintf(f,
            "OP=%s | STATE=%d | POS=%d | INPUT=%s | STACK=%s | ACTION=%s\n",
            operation,
            state,
            input_pos,
            remaining_input,
            stack,
            action);
}
