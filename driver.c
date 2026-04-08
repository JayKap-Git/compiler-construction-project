// Group NO.: 3
// Rudra Kaitan 2022B1A70849P
// Bhuvan Arora 2023A7PS0246P
// Prakriti Manocha 2023A7PS0672P
// Anshul Gupta 2023A7PS0587P
// Himanshu Singla 2023A7PS0522P
// Jayant Kapoor 2023A7PS1099P


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"

/* ================================================================
   Implementation Status (printed at startup)
   ================================================================ */
static void printStatus(void) {
    printf("=============================================================\n");
    printf("  Compiler Stage 1  —  Lexical and Syntactic Analysis\n");
    printf("=============================================================\n");
    printf("  Implementation Status:\n");
    printf("  (a) FIRST and FOLLOW sets automated\n");
    printf("  (c) Both lexical and syntax analysis modules implemented\n");
    printf("  Parse tree constructed via LL(1) predictive parsing\n");
    printf("  Panic-mode (SYNCH) error recovery implemented\n");
    printf("=============================================================\n\n");
}

/* ================================================================
   Option 1 — Remove comments, print comment-free code on console
   ================================================================ */
static void opt1_removeComments(const char *srcFile) {
    FILE *fp = fopen(srcFile, "r");
    if (!fp) {
        printf("Error: Cannot open '%s'\n", srcFile);
        return;
    }

    printf("\n--- Comment-Free Source Code ---\n");
    char line[2048];
    while (fgets(line, sizeof(line), fp)) {
        /* Locate first non-whitespace character */
        int i = 0;
        while (line[i] == ' ' || line[i] == '\t') i++;

        if (line[i] == '%') {
            /* Entire line is a comment — skip it */
            printf("\n");
            continue;
        }

        /* Strip inline comment: truncate at first '%' */
        for (int j = 0; line[j] != '\0'; j++) {
            if (line[j] == '%') {
                line[j]   = '\n';
                line[j+1] = '\0';
                break;
            }
        }
        printf("%s", line);
    }
    fclose(fp);
}

/* ================================================================
   Option 2 — Lexical analysis only; pretty-print token list
   ================================================================ */
static void opt2_printTokens(const char *srcFile) {
    FILE *fp = fopen(srcFile, "r");
    if (!fp) {
        printf("Error: Cannot open '%s'\n", srcFile);
        return;
    }

    initSymbolTable();

    twinBuffer B;
    initLexer(fp, &B);

    /* Header */
    printf("\n%-10s  %-30s  %s\n",
           "LINE NO.", "TOKEN TYPE", "LEXEME");
    printf("%-10s  %-30s  %s\n",
           "----------", "------------------------------",
           "--------------------");

    tokenInfo tok;
    do {
        tok = getNextToken(&B, fp);
        if (tok.type== TK_ERROR) continue;
        if (tok.type == TK_EOF)     break;
        printf("Line no. %-5d Lexeme %-20s Token %-15s\n", 
                tok.line_number, 
                tok.lexeme, 
                tokenNames[tok.type]);
    } while (1);

    fclose(fp);
}

/* ================================================================
   Option 3 — Parse source code; errors on console, tree in file
   ================================================================ */
static void opt3_parse(const char *srcFile, const char *outFile,
                       grammar *G, table T) {
    printf("\nParsing '%s' ...\n", srcFile);

    initSymbolTable();

    parseTree PT = parseInputSourceCode((char *)srcFile, T, G);
    if (PT == NULL) {
        printf("Error: Parsing failed (could not open source file).\n");
        return;
    }

    printParseTree(PT, (char *)outFile);
    printf("\nParse tree written to '%s'\n", outFile);
}

/* ================================================================
   Option 4 — Time the lexer + parser
   ================================================================ */
static void opt4_timing(const char *srcFile, const char *outFile,
                        grammar *G, table T) {
    clock_t start_time, end_time;
    double  total_CPU_time, total_CPU_time_in_seconds;

    initSymbolTable();

    printf("\nTiming lexer + parser on '%s' ...\n", srcFile);

    start_time = clock();

    /* Invoke lexer and parser */
    parseTree PT = parseInputSourceCode((char *)srcFile, T, G);

    end_time = clock();

    total_CPU_time            = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;

    printf("\n--- Timing Results ---\n");
    printf("Total CPU time (clock ticks) : %.0f\n", total_CPU_time);
    printf("Total CPU time (seconds)     : %.6f\n",  total_CPU_time_in_seconds);

    if (PT != NULL) {
        printParseTree(PT, (char *)outFile);
        printf("Parse tree written to '%s'\n", outFile);
    }
}

/* ================================================================
   main
   ================================================================ */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <sourceFile> <parseTreeOutFile>\n", argv[0]);
        return 1;
    }

    const char *srcFile = argv[1];
    const char *outFile = argv[2];

    /* --- Display implementation status --- */
    printStatus();

    /* --- Pre-compute grammar, FIRST/FOLLOW sets, parse table (once) --- */
    grammar G;
    initGrammar(&G);
    printf("Grammar loaded        (%d rules)\n", G.numRules);

    FirstAndFollow F = ComputeFirstAndFollowSets(G);
    printf("FIRST/FOLLOW sets     computed\n");

    table T;
    createParseTable(F, T, &G);
    printf("Parse table           built\n\n");

    /* --- Menu loop --- */
    int choice;
    do {
        printf("=========================================\n");
        printf("  0 : Exit\n");
        printf("  1 : Remove comments (print to console)\n");
        printf("  2 : Token list (lexical analysis only)\n");
        printf("  3 : Parse source code + print parse tree\n");
        printf("  4 : CPU time for lexer and parser\n");
        printf("=========================================\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = -1;
        }

        switch (choice) {
            case 0:
                printf("Exiting. Goodbye!\n");
                break;
            case 1:
                opt1_removeComments(srcFile);
                break;
            case 2:
                opt2_printTokens(srcFile);
                break;
            case 3:
                opt3_parse(srcFile, outFile, &G, T);
                break;
            case 4:
                opt4_timing(srcFile, outFile, &G, T);
                break;
            default:
                printf("Invalid option '%d'. Please enter 0-4.\n", choice);
                break;
        }
    } while (choice != 0);

    return 0;
}
