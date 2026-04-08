// Group NO.: 3
// Rudra Kaitan 2022B1A70849P
// Bhuvan Arora 2023A7PS0246P
// Prakriti Manocha 2023A7PS0672P
// Anshul Gupta 2023A7PS0587P
// Himanshu Singla 2023A7PS0522P
// Jayant Kapoor 2023A7PS1099P


#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

FirstAndFollow ComputeFirstAndFollowSets (grammar G);
void createParseTable(FirstAndFollow F, table T, grammar *G);
parseTree parseInputSourceCode(char *testcaseFile, table T,grammar* G);
void printParseTree(parseTree PT, char *outfile);

//helper functions
void initParseTable(table T);
void initFirstAndFollow(FirstAndFollow *F);
void initGrammar(grammar *G);
ParseTreeNode* createNewNode(int symbolID, int isTerminal);
void push(Stack *top, Symbol s, struct ParseTreeNode *nodePtr);
void pop(Stack *top);
StackNode* peek(Stack top);
int isEmpty(Stack top);

#endif