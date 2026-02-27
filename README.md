# P3 Bottom-up Parser Guide (Group GJ)

This document is an updated P3 guide for the current implementation.



## 👥 **Group Members**
- **Alex Martínez**     — Team Leader  (TM1)
- **Adrià Coll**        — Team Leader  (TM2)
- **Isabel Expósito**   — Team Leader  (TM3)
- **Didac Raya**        — Team Leader  (TM4)
- **Margarita Crespo**  — Team Leader  (TM5)
- **Valentyn Korol**    — Team Leader  (TM6)


GitHub-Repo: ([Compilers_P3_GJ](https://github.com/IsabelExpositoCastro/Compilers-P3-GJ.git))



## 📁 **Project Structure**

The project skeleton with all folders and files is the following one:
```
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

`OP=<SHIFT|REDUCE|ACCEPT|ERROR> | STATE=<state> | POS=<input_pos> | INPUT=<remaining_input$> | STACK=<stack_content> | ACTION=<human_readable_action>`

## 4) Language specification format

A language file must include these sections:

1. `LANGUAGE`
2. Grammar rules (using `_` as arrow)
3. Terminals line
4. Non-terminals line
5. `AUTOMATA`
6. Metadata: total states, start state, accept state
7. `ACTION` table
8. `GOTO` table

### Minimal structure example

```txt
LANGUAGE
s _ e
e _ e + t
e _ t
t _ t * f
t _ f
f _ (e)
f _ NUM
+ * ( ) NUM
s e t f

AUTOMATA
12
0
1

ACTION
STATE NUM + * ( ) $
...

GOTO
STATE s e t f
...
```

## 5) Supported language files in this repo

- `data/language.txt` (main grammar from P3 handout)
- `data/language2.txt` (alternate grammar format requested in handout context)

## 6) Main code modules

- `src/InArgs_Module`: argument parsing, input/output filename handling
- `src/Language_Module`: grammar loading and symbol mapping
- `src/Automata_Module`: automaton table loading and ACTION/GOTO queries
- `src/TokenReader_Module`: `.cscn` token stream loading and debug step writer
- `src/Stack_Module`: parser stack operations
- `src/Parser_Module`: shift/reduce execution engine
- `src/main.c`: end-to-end orchestration

## 7) Quick run examples

From `build/`:

- Main language:
  - `./parser.exe ../data/language.txt ../data/input_test.cscn`
- Alternate language:
  - `./parser.exe ../data/language2.txt ../data/input_lang2_valid.cscn`

## 8) Handout-oriented checklist (implementation status)

- General S/R engine: **Implemented**
- Grammar as data structure: **Implemented**
- Automaton tables as data (file-based): **Implemented**
- 2-argument usage (language + input): **Implemented**
- `.cscn` token input loading: **Implemented**
- Debug output per parser step: **Implemented**
- Output naming `_p3dbg.txt`: **Implemented**
- Multi-expression input file support: **Implemented**
- Extra test/integration coverage: **Partially implemented**

## 9) Notes

- The parser currently reports expression-level rejections in debug output while keeping process completion successful when execution itself is correct.
- For grading/demo, include both valid and invalid expressions to showcase ACCEPT and ERROR traces.
