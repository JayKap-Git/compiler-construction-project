// Group NO.: 3
// Rudra Kaitan 2022B1A70849P
// Bhuvan Arora 2023A7PS0246P
// Prakriti Manocha 2023A7PS0672P
// Anshul Gupta 2023A7PS0587P
// Himanshu Singla 2023A7PS0522P
// Jayant Kapoor 2023A7PS1099P


#ifndef LEXER_H
#define LEXER_H
#include "lexerDef.h"
FILE *getStream(FILE *fp, char *bufferSegment, int size);
tokenInfo getNextToken(twinBuffer *B, FILE *fp);
void removeComments(char *testcaseFile, char *cleanFile);

unsigned int hash(char *str);
SymbolEntry* lookup(char *lexeme);
SymbolEntry* insert(char *lexeme, TokenType tokenType);
void initSymbolTable();
void initLexer(FILE *fp, twinBuffer *B);
#endif