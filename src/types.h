// archivo para los tipos compartidos
// creo que tiene sentido tener todos los enums que usan todos los modulos 

// son los de tipo terminal, actiontype y symbolkind 

#ifndef TYPES_H
#define TYPES_H

// Terminals of the grammar (
typedef enum {
    TOK_NUM = 0,
    TOK_PLUS,
    TOK_STAR,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_DOLLAR,
    TOK_INVALID
} Terminal;

// ACTION types in the parsing table
typedef enum {
    ACT_SHIFT = 0,
    ACT_REDUCE,
    ACT_ACCEPT,
    ACT_ERROR
} ActionType;

// Symbol kind stored in stack / grammar rhs
typedef enum {
    SYM_TERMINAL = 1,
    SYM_NONTERMINAL = 2
} SymbolKind;

#endif