# P3 Bottom-up Parser Guide (Group GJ)

This document is an updated P3 guide for the current implementation.



## 👥 **Group Members**
| Member | NIA |  |
|---|---:|---|
| Alex Martínez | 253823 | Team Leader (TM1) |
| Adrià Coll | 253640 | Team Leader (TM2) |
| Isabel Expósito | 268790 | Team Leader (TM3) |
| Didac Raya | 242597 | Team Leader (TM4) |
| Margarita Crespo | 268352 | Team Leader (TM5) |
| Valentyn Korol | 253887 | Team Leader (TM6) |


GitHub-Repo: ([Compilers_P3_GJ](https://github.com/IsabelExpositoCastro/Compilers-P3-GJ.git))



## 📁 **Project Structure**

The project skeleton with all folders and files is the following one:
```
/data
│   tokens_file_p3dbg.txt
│   language.txt
│   tokens_file.cscn
│   parser.exe
│
/src
│   CMakeLists.txt
│   main.c
│   main.h
│   types.c
│   types.h
│   utils_files.c
│   utils_files.h
│   
├───Automata_Module
│       automata.c
│       automata.h
│       CMakeLists.txt
│       
├───InArgs_Module
│       CMakeLists.txt
│       inArgs.c
│       inArgs.h
│
├───Language_Module
│       CMakeLists.txt
│       language.c
│       language.h
│
├───Parser_Module
│       CMakeLists.txt
│       parser.c
│       parser.h
│
├───Stack_Module
│       CMakeLists.txt
│       stack.c
│       stack.h
│
└───TokenReader_Module
        CMakeLists.txt
        tokenReader.c
        tokenReader.h
```



## 1) What this project does

- Implements a general Shift/Reduce parser engine.
- Loads grammar rules from a language specification file.
- Loads ACTION/GOTO tables from the same language file.
- Loads token streams from a `.cscn` input file.
- Produces a debug trace file named `<input_basename>_p3dbg.txt`.

## 2) Current executable contract

The executable requires exactly 2 parameters:

1. Language specification file (grammar + automaton tables)
2. Input token stream file (`.cscn`)

Example (from `build/`):

`./parser.exe ../data/language.txt ../data/input_test.cscn`

## 3) Input and output files

### Input: token stream (`.cscn`)

Expected tuple format per token:

`<lexeme, CATEGORY>`

Examples of categories used:

- `CAT_NUMBER`
- `CAT_OPERATOR`
- `CAT_SPECIALCHAR`

You can place multiple expressions in the same input file (one expression per line of tuples).

### Output: parser debug trace (`_p3dbg.txt`)

Output file name rule:

`<input_filename_without_extension>_p3dbg.txt`

Example:

- Input: `input_test.cscn`
- Output: `input_test_p3dbg.txt`

Each expression block includes:

- `Expression N`
- `EXPR=<full_expression_reconstructed>`
- One-line step traces in the format:

`<OP> | <STATE> | <POS> | <INPUT$> | <STACK> | <ACTION_TEXT>`

Where:

- `OP` is one of `S`, `R`, `A`, `E`
- `ACTION_TEXT` includes detailed stack transitions:
  - `Shift from state X to state Y | PUSH [sym, state]`
  - `Reduce by Rn (...) | POP {...} | PUSH [lhs, state] | GOTO [from, lhs]=to`

Example:

`R | 5 | 2 | +3)*5$ | [0, (, 4, NUM, 5] | Reduce by R7 (f -> NUM) | POP {[NUM, 5]} | PUSH [f, 3] | GOTO [4, f]=3`

---
### Examples:
**In 📁/data folder, you can find both tokens_file.cscn & language.txt. Also, there is the executable with the resulting outputFile (tokens_file_p3dbg.txt)**



## 4) Language specification format

A language file must include these sections:

1. `LANGUAGE`
2. Terminals line
3. Non-terminals line
4. `RULES`
5. Grammar rules (using `_` as arrow)
6. `AUTOMATA`
7. Metadata: total states, start state, accept state
8. `ACTION` table
9. `GOTO` table

### Minimal structure example

```txt
LANGUAGE
 + * ( ) NUM
 s e t f

RULES
s _ e
e _ e + t
e _ t
t _ t * f
t _ f
f _ (e)
f _ NUM

AUTOMATA
22
0
1

ACTION
STATE NUM + * ( ) $
...

GOTO
STATE s e t f
...
```

Note: the current `load_grammar_from_file` implementation is tolerant and can still parse legacy layouts as long as terminals, non-terminals, and rules are present before `AUTOMATA`.

## 5) Supported language files in this repo

- `data/language.txt` (main grammar + 22-state automaton)
- `data/language_complete.txt` (same grammar/automaton in a separate complete file)
- `data/language2.txt` (alternate grammar used in previous tests)

## 6) Main code modules

- `src/InArgs_Module`: argument parsing, input/output filename handling
- `src/Language_Module`: grammar loading and symbol mapping
- `src/Automata_Module`: automaton table loading and ACTION/GOTO queries
- `src/TokenReader_Module`: `.cscn` token stream loading and debug step writer
- `src/Stack_Module`: parser stack operations
- `src/Parser_Module`: shift/reduce execution engine
- `src/main.c`: end-to-end orchestration

## 7) Usage Instructions
In order to execute successfully the P3-Parser practice, using the terminal, it is mandatory to access to /data folder and execute the following command:

```
./parser.exe language.txt tokens_file.cscn
```

## 8) Requirements
As a team, we have mostly achieved the complete practice goals. However, there are some requirements that are missing:
- Functions Display 
- Code Documentation
- Team work distribution specification


## 9) Submission Status

| Task | Status |
|---|---|
| Automata for the Language 1 | **Done** |
| Automata for the Language 2 | **Done** |
| Definition of the input language | **Done** |
| Implement Language Data Struct | **Done** |
| Decide Token Struct | **Done** |
| Implement Token Struct | **Done** |
| Stack Data Struct | **Done** |
| Implement Stack Data Struct | **Done** |
| Output format | **Done** |
| Initial program design | **Done** |
| Automata module | **Done** |
| Language loader module | **Done** |
| Token module | **Done** |
| Input handler module | **Done** |
| Token Reader module | **Done** |
| Scanner (parser) module | **Done** |
| Stack Handler module | **Done** |
| Definition of the input file | **Done** |

