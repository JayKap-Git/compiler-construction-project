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
#include <ctype.h>
#include <math.h>
#include "parser.h"
#include "lexer.h"

void initGrammar(grammar *G) {
    memset(G, 0, sizeof(grammar));

    FILE *fp = fopen("grammar.txt", "r");
    if (fp == NULL) {
        printf("Error: Could not open grammar file grammar.txt\n");
        G->numRules = 0;
        return;
    }

    char line[1024];
    int ruleCount = 0;
    int lineNo = 0;

    while (fgets(line, sizeof(line), fp) != NULL) {
        lineNo++;

        line[strcspn(line, "\r\n")] = '\0';

        char *p = line;
        while (*p && isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;

        char *arrow = strstr(p, "->");
        if (arrow == NULL) {
            printf("Error in grammar.txt at line %d: missing ->\n", lineNo);
            fclose(fp);
            G->numRules = 0;
            return;
        }

        if (ruleCount >= MAX_RULES) {
            printf("Error in grammar.txt: number of rules exceeds MAX_RULES (%d)\n", MAX_RULES);
            fclose(fp);
            G->numRules = 0;
            return;
        }

        *arrow = '\0';
        char *lhsRaw = p;
        char *rhsRaw = arrow + 2;

        while (*lhsRaw && isspace((unsigned char)*lhsRaw)) lhsRaw++;
        char *lhsEnd = lhsRaw + strlen(lhsRaw) - 1;
        while (lhsEnd >= lhsRaw && isspace((unsigned char)*lhsEnd)) {
            *lhsEnd = '\0';
            lhsEnd--;
        }

        if (lhsRaw[0] != '<') {
            printf("Error in grammar.txt at line %d: invalid LHS %s\n", lineNo, lhsRaw);
            fclose(fp);
            G->numRules = 0;
            return;
        }

        char *gt = strchr(lhsRaw, '>');
        if (gt == NULL) {
            printf("Error in grammar.txt at line %d: malformed non-terminal %s\n", lineNo, lhsRaw);
            fclose(fp);
            G->numRules = 0;
            return;
        }

        char lhsName[128];
        int lhsLen = (int)(gt - lhsRaw - 1);
        if (lhsLen <= 0 || lhsLen >= (int)sizeof(lhsName)) {
            printf("Error in grammar.txt at line %d: invalid LHS length\n", lineNo);
            fclose(fp);
            G->numRules = 0;
            return;
        }
        strncpy(lhsName, lhsRaw + 1, lhsLen);
        lhsName[lhsLen] = '\0';

        int lhsId = -1;
        for (int i = 0; i < NUM_NON_TERMINALS; i++) {
            if (strcmp(nonTerminalNames[i], lhsName) == 0) {
                lhsId = i + 100;
                break;
            }
        }

        if (lhsId == -1) {
            printf("Error in grammar.txt at line %d: unknown non-terminal <%s>\n", lineNo, lhsName);
            fclose(fp);
            G->numRules = 0;
            return;
        }

        GrammarRule *rule = &G->rules[ruleCount];
        rule->lhs = lhsId;
        rule->rhsSize = 0;

        while (*rhsRaw && isspace((unsigned char)*rhsRaw)) rhsRaw++;

        char *tok = strtok(rhsRaw, " \t");
        int sawEps = 0;
        while (tok != NULL) {
            if (strcmp(tok, "eps") == 0) {
                sawEps = 1;
                tok = strtok(NULL, " \t");
                if (tok != NULL) {
                    printf("Error in grammar.txt at line %d: eps must be the only RHS symbol\n", lineNo);
                    fclose(fp);
                    G->numRules = 0;
                    return;
                }
                break;
            }

            if (rule->rhsSize >= MAX_RHS) {
                printf("Error in grammar.txt at line %d: RHS exceeds MAX_RHS (%d)\n", lineNo, MAX_RHS);
                fclose(fp);
                G->numRules = 0;
                return;
            }

            if (tok[0] == '<') {
                char *tokGt = strchr(tok, '>');
                if (tokGt == NULL) {
                    printf("Error in grammar.txt at line %d: malformed RHS non-terminal %s\n", lineNo, tok);
                    fclose(fp);
                    G->numRules = 0;
                    return;
                }

                char ntName[128];
                int ntLen = (int)(tokGt - tok - 1);
                if (ntLen <= 0 || ntLen >= (int)sizeof(ntName)) {
                    printf("Error in grammar.txt at line %d: invalid RHS non-terminal length\n", lineNo);
                    fclose(fp);
                    G->numRules = 0;
                    return;
                }

                strncpy(ntName, tok + 1, ntLen);
                ntName[ntLen] = '\0';

                int ntId = -1;
                for (int i = 0; i < NUM_NON_TERMINALS; i++) {
                    if (strcmp(nonTerminalNames[i], ntName) == 0) {
                        ntId = i + 100;
                        break;
                    }
                }

                if (ntId == -1) {
                    printf("Error in grammar.txt at line %d: unknown RHS non-terminal <%s>\n", lineNo, ntName);
                    fclose(fp);
                    G->numRules = 0;
                    return;
                }

                rule->rhs[rule->rhsSize++] = (Symbol){ntId, 0};
            } else {
                int tkId = -1;
                for (int i = 0; i < NUM_TERMINALS; i++) {
                    if (strcmp(tokenNames[i], tok) == 0) {
                        tkId = i;
                        break;
                    }
                }

                if (tkId == -1) {
                    printf("Error in grammar.txt at line %d: unknown terminal %s\n", lineNo, tok);
                    fclose(fp);
                    G->numRules = 0;
                    return;
                }

                rule->rhs[rule->rhsSize++] = (Symbol){tkId, 1};
            }

            tok = strtok(NULL, " \t");
        }

        if (!sawEps && rule->rhsSize == 0) {
            printf("Error in grammar.txt at line %d: empty RHS is not allowed (use eps)\n", lineNo);
            fclose(fp);
            G->numRules = 0;
            return;
        }

        ruleCount++;
    }

    fclose(fp);
    G->numRules = ruleCount;
}


void initParseTable(table T) {
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            T[i][j] = -1; //Represents an eror entry
        }
    }
}
void initFirstAndFollow(FirstAndFollow *F) {
    memset(F, 0, sizeof(FirstAndFollow)); //Ensures clean First and Follow sets
}

FirstAndFollow ComputeFirstAndFollowSets (grammar G){
    FirstAndFollow F;
    initFirstAndFollow(&F);
    //Arrays are 0-based, NT enum values start at 100
    #define NT_IDX(nt) ((nt) - 100)
    //First Sets
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < G.numRules; i++) {
            GrammarRule *rule = &G.rules[i];
            int lhs = NT_IDX(rule->lhs);
            //Epsilon rule
            if (rule->rhsSize == 0) {
                if (!F.hasEpsilon[lhs]) { F.hasEpsilon[lhs] = 1; changed = 1; }
                continue;
            }
            //stop when a symbol cannot derive epsilon
            int allEps = 1;
            for (int j = 0; j < rule->rhsSize; j++) {
                Symbol s = rule->rhs[j];
                if (s.isTerminal) {
                    //FIRST(lhs)= {s}
                    if (!F.first[lhs][s.id]) { F.first[lhs][s.id] = 1; changed = 1; }
                    allEps = 0;
                    break;
                }
                //s is a non-terminal, FIRST(lhs)=FIRST(s)−{ε}
                int sIdx = NT_IDX(s.id);
                for (int t = 0; t < NUM_TERMINALS; t++) {
                    if (F.first[sIdx][t] && !F.first[lhs][t]) {
                        F.first[lhs][t] = 1; changed = 1;
                    }
            }
                if (!F.hasEpsilon[sIdx]) { allEps = 0; break; }
                //else, s can vanish, go to the next symbol
            }
            //Every symbol on the RHS could derive epsilon
            if (allEps && !F.hasEpsilon[lhs]) { F.hasEpsilon[lhs] = 1; changed = 1; }
        }
    }

    //Compute Follow sets
    //Rule, FOLLOW(start) contains TK_EOF
    F.follow[NT_IDX(NT_PROGRAM)][TK_EOF] = 1;

    changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < G.numRules; i++) {
            GrammarRule *rule = &G.rules[i];
            int lhs = NT_IDX(rule->lhs);
            for (int j = 0; j < rule->rhsSize; j++) {
                Symbol s = rule->rhs[j];
                if (s.isTerminal) continue;   //only non terminals get FOLLOW set
                int sIdx = NT_IDX(s.id);
                //Look at everything that appears after s in this production.
                //Add FIRST(of remaining part) to FOLLOW(s), do not add epsilon.
                //if there is nothing in remaining part,
                //also copy FOLLOW(lhs) into FOLLOW(s).
                int suffixAllEps = 1;
                for (int k = j + 1; k < rule->rhsSize; k++) {
                    Symbol nx = rule->rhs[k];
                    if (nx.isTerminal) {
                        if (!F.follow[sIdx][nx.id]) { F.follow[sIdx][nx.id] = 1; changed = 1; }
                        suffixAllEps = 0;
                        break;
                    }
                    int nxIdx = NT_IDX(nx.id);
                    for (int t = 0; t < NUM_TERMINALS; t++) {
                        if (F.first[nxIdx][t] && !F.follow[sIdx][t]) {
                            F.follow[sIdx][t] = 1; changed = 1;
                        }
                    }
                    if (!F.hasEpsilon[nxIdx]) { suffixAllEps = 0; break; }
                }

                //If there is nothing after this symbol, or everything after it can produce empty,
                //then whatever can follow the lhs can also follow this symbol.
                if (suffixAllEps) {
                    for (int t = 0; t < NUM_TERMINALS; t++) {
                        if (F.follow[lhs][t] && !F.follow[sIdx][t]) {
                            F.follow[sIdx][t] = 1; changed = 1;
                        }
                    }
                }
            }
        }
    }

    #undef NT_IDX
    return F;
}

void createParseTable(FirstAndFollow F, table T, grammar *G){
    initParseTable(T);   //fill every cell with -1

    #define NT_IDX(nt) ((nt) - 100)
    //used for panic-mode pop actions.
    static int syncAnchor[NUM_TERMINALS] = {0};
    static int syncAnchorInit = 0;
    if (!syncAnchorInit) {
        syncAnchor[TK_ENDRECORD] = 1;
        syncAnchor[TK_ENDUNION]  = 1;
        syncAnchor[TK_ENDIF]     = 1;
        syncAnchor[TK_ELSE]      = 1;   
        syncAnchor[TK_ENDWHILE]  = 1;
        syncAnchor[TK_CL]        = 1;
        syncAnchor[TK_SQR]       = 1;
         syncAnchor[TK_SEM]       = 1;
         syncAnchor[TK_MAIN]      = 1;
         // syncAnchor[TK_END]      = 1;
        syncAnchor[TK_EOF]       = 1;
        syncAnchorInit = 1;
    }

    //Fill valid rule entries
    for (int i = 0; i < G->numRules; i++) {
        GrammarRule *rule = &G->rules[i];
        int lhs = NT_IDX(rule->lhs);

        //Epsilon rule
        if (rule->rhsSize == 0) {
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (F.follow[lhs][t])
                    T[lhs][t] = i;
            }
            continue;
        }

        //Walk  RHS, compute FIRST
        int rhsAllEps = 1;
        for (int j = 0; j < rule->rhsSize; j++) {
            Symbol s = rule->rhs[j];
            if (s.isTerminal) {
                T[lhs][s.id] = i;
                rhsAllEps = 0;
                break;
            }
            //s is a non-terminal, add FIRST(s) − epsilon
            int sIdx = NT_IDX(s.id);
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (F.first[sIdx][t])
                    T[lhs][t] = i;
            }
            if (!F.hasEpsilon[sIdx]) { rhsAllEps = 0; break; }
            //else, s can vanish
        }

        //Entire RHS can derive epsilon
        if (rhsAllEps) {
            for (int t = 0; t < NUM_TERMINALS; t++) {
                if (F.follow[lhs][t])
                    T[lhs][t] = i;
            }
        }
    }

    //STEP 2, Error recovery
    for (int a = 0; a < NUM_NON_TERMINALS; a++) {
        for (int t = 0; t < NUM_TERMINALS; t++) {
            if (T[a][t] == -1 && (F.follow[a][t] && syncAnchor[t]))
                T[a][t] = SYNCH;
            if (T[a][t] == -1 && t == TK_MAIN)
                T[a][t] = SYNCH;
        }
    }

    #undef NT_IDX
}

ParseTreeNode* createNewNode(int symbolID, int isTerminal) {
    ParseTreeNode* newNode = (ParseTreeNode*)malloc(sizeof(ParseTreeNode));
    newNode->s.id = symbolID;
    newNode->s.isTerminal = isTerminal;
    newNode->child = NULL;
    newNode->sibling = NULL;
    newNode->parent = NULL;
    newNode->token = NULL; 
    return newNode;
}

//stack functions
void push(Stack *top, Symbol s, struct ParseTreeNode *nodePtr) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    newNode->s = s;
    newNode->nodePtr = nodePtr;
    newNode->next = *top;
    *top = newNode;
}
void pop(Stack *top) {
    if (*top == NULL) return;
    StackNode *temp = *top;
    *top = (*top)->next;
    free(temp);
}
StackNode* peek(Stack top) {
    return top;
}
int isEmpty(Stack top) {
    return top == NULL;
}

//actual parsing is happening here
parseTree parseInputSourceCode(char *testcaseFile, table T,grammar* G){
    //we will open the source code
    FILE *fp = fopen(testcaseFile, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", testcaseFile);
        return NULL;
    }

    //Initialize the Lexer
    twinBuffer B; 
    initLexer(fp, &B);

    //Get the first non-comment, non-error lookahead token
    tokenInfo lookahead = getNextToken(&B,fp);
    while (lookahead.type == TK_COMMENT || lookahead.type == TK_ERROR) lookahead = getNextToken(&B, fp);

    //SETUP PARSE TREE & STACK
    //Create the root of the tree (Starting Non-Terminal) 
    //here assuming NT_PROGRAM is start symbol
    parseTree PT = createNewNode(NT_PROGRAM, 0); 

    Stack S = NULL; 
    //Push EOF first, then the Start Symbol
    push(&S, (Symbol){TK_EOF, 1}, NULL); 
    push(&S, (Symbol){NT_PROGRAM, 0}, PT);

    //we need to write the full parcing logic here
    //MAIN PARSING LOOP
    int parsing = 1;
    int lastReportedLine = -1;
    int lastReportedType = -1;

    #define REPORT_SYNTAX_ONCE(_fmt, ...) \
        do { \
            if (lookahead.line_number != lastReportedLine || lookahead.type != lastReportedType) { \
                printf(_fmt, __VA_ARGS__); \
                lastReportedLine = lookahead.line_number; \
                lastReportedType = lookahead.type; \
            } \
        } while (0)

    while (!isEmpty(S) && parsing) {
        StackNode *topElement = peek(S);
        Symbol X = topElement->s;
        ParseTreeNode *currentNodeInTree = topElement->nodePtr;

        //Case 1, Top of stack is a Terminal
        if (X.isTerminal) {
            if (X.id == lookahead.type) {
                //Match, store the token info in the leaf node
                if (currentNodeInTree != NULL) {
                    currentNodeInTree->token = (struct tokenInfo*)malloc(sizeof(struct tokenInfo));
                    *(currentNodeInTree->token) = lookahead;
                }
                pop(&S);
                if (X.id != TK_EOF) {
                    do { lookahead = getNextToken(&B, fp); }
                    while (lookahead.type == TK_COMMENT || lookahead.type == TK_ERROR);
                }
            } else {
                //Mismatch, pop the expected terminal, do NOT advance lookahead.
                REPORT_SYNTAX_ONCE("Syntactical Error at Line %d : The token %s for lexeme %s does not match with the expected token %s\n",
                                   lookahead.line_number, tokenNames[lookahead.type], lookahead.lexeme, tokenNames[X.id]);
                pop(&S);
                if (lookahead.type == TK_EOF) break;
            }
        }

        //Case 2, Top of stack is a Non-Terminal
        else {
            int ntIdx    = X.id - 100;
            int ruleIdx  = T[ntIdx][lookahead.type];

            if (ruleIdx >= 0) {
                //Valid rule, pop NT and expand
                pop(&S);
                GrammarRule rule = G->rules[ruleIdx];

                if (rule.rhsSize == 0) {
                    ParseTreeNode *epsNode = createNewNode(TK_EPSILON, 1);
                    epsNode->parent = currentNodeInTree;
                    currentNodeInTree->child = epsNode;
                } else {
                    ParseTreeNode *children[MAX_RHS];
                    for (int i = 0; i < rule.rhsSize; i++) {
                        children[i] = createNewNode(rule.rhs[i].id, rule.rhs[i].isTerminal);
                        children[i]->parent = currentNodeInTree;
                        if (i == 0)
                            currentNodeInTree->child = children[i];
                        else
                            children[i-1]->sibling = children[i];
                    }
                    for (int i = rule.rhsSize - 1; i >= 0; i--)
                        push(&S, rule.rhs[i], children[i]);
                }

            } else if (ruleIdx == SYNCH) {
                REPORT_SYNTAX_ONCE("Syntactical Error at Line %d : Invalid token %s encountered with value %s stack top %s\n",
                                   lookahead.line_number, tokenNames[lookahead.type],
                                   lookahead.lexeme, nonTerminalNames[ntIdx]);
                pop(&S);

            } else {
                //panic-mode recovery.
                REPORT_SYNTAX_ONCE("Syntactical Error at Line %d : Invalid token %s encountered with value %s stack top %s\n",
                                   lookahead.line_number, tokenNames[lookahead.type],
                                   lookahead.lexeme, nonTerminalNames[ntIdx]);

                do {
                    if (lookahead.type == TK_EOF) { parsing = 0; break; }

                    do { lookahead = getNextToken(&B, fp); }
                    while (lookahead.type == TK_COMMENT || lookahead.type == TK_ERROR);

                    int action = T[ntIdx][lookahead.type];

                    if (action >= 0) {
                        //Token can start a valid derivation for this NT
                        break;
                    }
                    if (action == SYNCH) {
                        //Token is sync token for this NT
                        pop(&S);
                        break;
                    }
                    //Still blank error: keep skipping input tokens
                } while (1);
            }
        }
    }

    #undef REPORT_SYNTAX_ONCE


    fclose(fp);
    return PT;
}


//Helpers


static void inOrderTraversal(ParseTreeNode* node, FILE* fp);

void printNode(ParseTreeNode* node, FILE* fp) {
    if (node == NULL) return;

    if (node->s.isTerminal) {
        //Leaf node (terminal)
        const char* lexeme = "---";
        char lineStr[20];
        strcpy(lineStr, "---");
        const char* tokenName = tokenNames[node->s.id];
        char valueStr[64];
        strcpy(valueStr, "---");

        if (node->token != NULL) {
            lexeme = node->token->lexeme;
            sprintf(lineStr, "%d", node->token->line_number);
            tokenName = tokenNames[node->token->type];

            if (node->token->type == TK_NUM) {
                sprintf(valueStr, "%d", node->token->value.i_val);
            } else if (node->token->type == TK_RNUM) {
                sprintf(valueStr, "%f", node->token->value.r_val);
            }
        }

        //parentNodeSymbol
        const char* parentSymbol;
        if (node->parent == NULL) {
            parentSymbol = "ROOT";
        } else if (node->parent->s.isTerminal) {
            parentSymbol = tokenNames[node->parent->s.id];
        } else {
            parentSymbol = nonTerminalNames[node->parent->s.id - 100];
        }

        const char* nodeSymbol = tokenNames[node->s.id];

        fprintf(fp, "%-25s %-30s %-10s %-30s %-15s %-30s %-10s %-30s\n",
            lexeme,
            nodeSymbol,
            lineStr,
            tokenName,
            valueStr,
            parentSymbol,
            "yes",
            nodeSymbol
        );
    } else {
        //Non-terminal node
        const char* parentSymbol;
        if (node->parent == NULL) {
            parentSymbol = "ROOT";
        } else if (node->parent->s.isTerminal) {
            parentSymbol = tokenNames[node->parent->s.id];
        } else {
            parentSymbol = nonTerminalNames[node->parent->s.id - 100];
        }

        const char* nodeSymbol = nonTerminalNames[node->s.id - 100];

        fprintf(fp, "%-25s %-30s %-10s %-30s %-15s %-30s %-10s %-30s\n",
            "---",       //lexeme
            nodeSymbol,  //CurrentNode
            "---",       //lineno
            "---",       //tokenName
            "---",       //valueIfNumber
            parentSymbol,//parentNodeSymbol
            "no",        //isLeafNode
            nodeSymbol   //NodeSymbol
        );
    }
}

static void inOrderTraversal(ParseTreeNode* node, FILE* fp) {
    if (node == NULL) return;

    ParseTreeNode* leftmostChild = node->child;

    //Leftmost child first
    if (leftmostChild != NULL) {
        inOrderTraversal(leftmostChild, fp);
    }

    //Then parent
    printNode(node, fp);

    //Then remaining siblings of leftmost child
    if (leftmostChild != NULL) {
        ParseTreeNode* sibling = leftmostChild->sibling;
        while (sibling != NULL) {
            inOrderTraversal(sibling, fp);
            sibling = sibling->sibling;
        }
    }
}

    void printParseTree(parseTree PT, char *outfile) {
        if (PT == NULL || outfile == NULL) return;

        FILE* fp = fopen(outfile, "w");
        if (fp == NULL) {
            printf("Error: Could not open output file %s\n", outfile);
            return;
        }

        fprintf(fp, "%-25s %-30s %-10s %-30s %-15s %-30s %-10s %-30s\n",
            "lexeme",
            "CurrentNode",
            "lineno",
            "tokenName",
            "valueIfNumber",
            "parentNodeSymbol",
            "isLeafNode",
            "NodeSymbol"
        );
        fprintf(fp, "%-25s %-30s %-10s %-30s %-15s %-30s %-10s %-30s\n",
            "-------------------------",
            "------------------------------",
            "----------",
            "------------------------------",
            "---------------",
            "------------------------------",
            "----------",
            "------------------------------"
        );

        //In-order traversal
        inOrderTraversal(PT, fp);
        fclose(fp);
    }