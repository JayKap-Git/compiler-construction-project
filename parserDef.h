// Group NO.: 3
// Rudra Kaitan 2022B1A70849P
// Bhuvan Arora 2023A7PS0246P
// Prakriti Manocha 2023A7PS0672P
// Anshul Gupta 2023A7PS0587P
// Himanshu Singla 2023A7PS0522P
// Jayant Kapoor 2023A7PS1099P


#ifndef PARSER_DEF_H
#define PARSER_DEF_H

//Including the Lexer definitions because your ParseTree and Symbol need TokenType and tokenInfo.
#include"lexerDef.h"


typedef enum {
    NT_PROGRAM = 100,
    NT_MAIN_FUNCTION,
    NT_OTHER_FUNCTIONS,
    NT_FUNCTION,
    NT_INPUT_PAR,
    NT_OUTPUT_PAR,
    NT_PARAMETER_LIST,
    NT_DATA_TYPE,
    NT_PRIMITIVE_DATATYPE,
    NT_CONSTRUCTED_DATATYPE,
    NT_REMAINING_LIST,
    NT_STMTS,
    NT_TYPE_DEFINITIONS,
    NT_ACTUAL_OR_REDEFINED,
    NT_TYPE_DEFINITION,
    NT_FIELD_DEFINITIONS,
    NT_FIELD_DEFINITION,
    NT_FIELD_TYPE,
    NT_MORE_FIELDS,
    NT_DECLARATIONS,
    NT_DECLARATION,
    NT_GLOBAL_OR_NOT,
    NT_OTHER_STMTS,
    NT_STMT,
    NT_ASSIGNMENT_STMT,
    NT_ONE_EXPANSION,
    NT_MORE_EXPANSIONS,
    NT_SINGLE_OR_REC_ID,
    NT_OPTION_SINGLE_CONSTRUCTED,
    NT_FUN_CALL_STMT,
    NT_OUTPUT_PARAMETERS,
    NT_INPUT_PARAMETERS,
    NT_ITERATIVE_STMT,
    NT_CONDITIONAL_STMT,
    NT_ELSE_PART,
    NT_IO_STMT,
    NT_ARITHMETIC_EXPRESSION,
    NT_EXP_PRIME,
    NT_TERM,
    NT_TERM_PRIME,
    NT_FACTOR,
    NT_HIGH_PRECEDENCE_OPERATORS,
    NT_LOW_PRECEDENCE_OPERATORS,
    NT_BOOLEAN_EXPRESSION,
    NT_VAR,
    NT_LOGICAL_OP,
    NT_RELATIONAL_OP,
    NT_RETURN_STMT,
    NT_OPTIONAL_RETURN,
    NT_ID_LIST,
    NT_MORE_IDS,
    NT_DEFINE_TYPE_STMT,
    NT_A,
} NonTerminal;

    static const char *nonTerminalNames[] = {
        [NT_PROGRAM               - 100] = "program",
        [NT_MAIN_FUNCTION         - 100] = "mainFunction",
        [NT_OTHER_FUNCTIONS       - 100] = "otherFunctions",
        [NT_FUNCTION              - 100] = "function",
        [NT_INPUT_PAR             - 100] = "input_par",
        [NT_OUTPUT_PAR            - 100] = "output_par",
        [NT_PARAMETER_LIST        - 100] = "parameter_list",
        [NT_DATA_TYPE             - 100] = "dataType",
        [NT_PRIMITIVE_DATATYPE    - 100] = "primitiveDatatype",
        [NT_CONSTRUCTED_DATATYPE  - 100] = "constructedDatatype",
        [NT_REMAINING_LIST        - 100] = "remaining_list",
        [NT_STMTS                 - 100] = "stmts",
        [NT_TYPE_DEFINITIONS      - 100] = "typeDefinitions",
        [NT_ACTUAL_OR_REDEFINED   - 100] = "actualOrRedefined",
        [NT_TYPE_DEFINITION       - 100] = "typeDefinition",
        [NT_FIELD_DEFINITIONS     - 100] = "fieldDefinitions",
        [NT_FIELD_DEFINITION      - 100] = "fieldDefinition",
        [NT_FIELD_TYPE            - 100] = "fieldType",
        [NT_MORE_FIELDS           - 100] = "moreFields",
        [NT_DECLARATIONS          - 100] = "declarations",
        [NT_DECLARATION           - 100] = "declaration",
        [NT_GLOBAL_OR_NOT         - 100] = "global_or_not",
        [NT_OTHER_STMTS           - 100] = "otherStmts",
        [NT_STMT                  - 100] = "stmt",
        [NT_ASSIGNMENT_STMT       - 100] = "assignmentStmt",
        [NT_ONE_EXPANSION         - 100] = "oneExpansion",
        [NT_MORE_EXPANSIONS       - 100] = "moreExpansions",
        [NT_SINGLE_OR_REC_ID      - 100] = "singleOrRecId",
        [NT_OPTION_SINGLE_CONSTRUCTED - 100] = "option_single_constructed",
        [NT_FUN_CALL_STMT         - 100] = "funCallStmt",
        [NT_OUTPUT_PARAMETERS     - 100] = "outputParameters",
        [NT_INPUT_PARAMETERS      - 100] = "inputParameters",
        [NT_ITERATIVE_STMT        - 100] = "iterativeStmt",
        [NT_CONDITIONAL_STMT      - 100] = "conditionalStmt",
        [NT_ELSE_PART             - 100] = "elsePart",
        [NT_IO_STMT               - 100] = "ioStmt",
        [NT_ARITHMETIC_EXPRESSION - 100] = "arithmeticExpression",
        [NT_EXP_PRIME             - 100] = "expPrime",
        [NT_TERM                  - 100] = "term",
        [NT_TERM_PRIME            - 100] = "termPrime",
        [NT_FACTOR                - 100] = "factor",
        [NT_HIGH_PRECEDENCE_OPERATORS - 100] = "highPrecedenceOperators",
        [NT_LOW_PRECEDENCE_OPERATORS  - 100] = "lowPrecedenceOperators",
        [NT_BOOLEAN_EXPRESSION    - 100] = "booleanExpression",
        [NT_VAR                   - 100] = "var",
        [NT_LOGICAL_OP            - 100] = "logicalOp",
        [NT_RELATIONAL_OP         - 100] = "relationalOp",
        [NT_RETURN_STMT           - 100] = "returnStmt",
        [NT_OPTIONAL_RETURN       - 100] = "optionalReturn",
        [NT_ID_LIST               - 100] = "idList",
        [NT_MORE_IDS              - 100] = "more_ids",
        [NT_DEFINE_TYPE_STMT      - 100] = "definetypestmt",
        [NT_A                     - 100] = "A",
    };
typedef struct {
    int id;           // This will hold either a TokenType or a NonTerminal value
    int isTerminal;   // 1 if id is a TokenType, 0 if id is a NonTerminal
} Symbol;

#define MAX_RHS 10
#define MAX_RULES 100
#define SYNCH -2   // Parse table sentinel: pop NT and sync on this token

typedef struct {
    int lhs;                  // Non-Terminal ID
    Symbol rhs[MAX_RHS];      // Array of symbols
    int rhsSize;              // Number of symbols on the RHS
} GrammarRule;

typedef struct {
    GrammarRule rules[MAX_RULES];
    int numRules;
} grammar;


#define NUM_NON_TERMINALS 53  // One per non-terminal in rules.txt
#define NUM_TERMINALS 60      // Based on your TokenType enum size

typedef struct {
    // A 2D array where [i][j] is 1 if terminal j is in the set of non-terminal i
    unsigned char first[NUM_NON_TERMINALS][NUM_TERMINALS];
    unsigned char follow[NUM_NON_TERMINALS][NUM_TERMINALS];
    
    // Helper flag to track if a Non-Terminal can derive Epsilon (ε)
    int hasEpsilon[NUM_NON_TERMINALS]; 
} FirstAndFollow;

typedef int table[NUM_NON_TERMINALS][NUM_TERMINALS];

typedef struct ParseTreeNode {
    Symbol s;                      // Contains id (Terminal/NT) and isTerminal flag
    struct tokenInfo *token;       // Pointer to lexer data (NULL if it's a Non-Terminal)
    
    struct ParseTreeNode *parent;  // Pointer to the parent node
    struct ParseTreeNode *child;   // Pointer to the first (leftmost) child
    struct ParseTreeNode *sibling; // Pointer to the immediate right sibling
    
    // Optional metadata for the project
    int ruleIndex;                 // The grammar rule that created this node
    int lineNum;                   // For error reporting and tree printing
} ParseTreeNode;

typedef ParseTreeNode* parseTree;


//stack functions
typedef struct StackNode {
    Symbol s;                  // The symbol (Terminal or Non-Terminal)
    struct ParseTreeNode *nodePtr; // Link to the actual node in the Parse Tree
    struct StackNode *next;
} StackNode;

typedef StackNode* Stack;





#endif