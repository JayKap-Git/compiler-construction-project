# Compiler Construction Project

Stage 1 compiler front end: **lexical analysis** and **syntactic analysis** with an LL(1) predictive parser, automated FIRST/FOLLOW sets, parse tree construction, and panic-mode (SYNCH) error recovery.

## Build

```bash
make
```

Produces the executable `stage1exe`.

## Clean

```bash
make clean
```

## Usage

Run `stage1exe` and follow the interactive menu to choose comment stripping, token listing, parse tree output, or time comparisons. Point the program at your source file when prompted (e.g. `sourceCode.txt` or the provided test inputs).

## Project layout

| File | Role |
|------|------|
| `lexer.c` / `lexer.h` / `lexerDef.h` | Lexer and token definitions |
| `parser.c` / `parser.h` / `parserDef.h` | Grammar, FIRST/FOLLOW, LL(1) parser |
| `driver.c` | CLI driver and options |
| `grammar.txt` | Grammar specification |
| `makefile` | Build rules |
