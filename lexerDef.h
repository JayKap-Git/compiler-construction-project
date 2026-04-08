// Group NO.: 3
// Rudra Kaitan 2022B1A70849P
// Bhuvan Arora 2023A7PS0246P
// Prakriti Manocha 2023A7PS0672P
// Anshul Gupta 2023A7PS0587P
// Himanshu Singla 2023A7PS0522P
// Jayant Kapoor 2023A7PS1099P


#ifndef LEXERDEF_H
#define LEXERDEF_H
#include <stdio.h>
typedef enum {
    /* --- Existing Tokens --- */
    TK_NUM,
    TK_RNUM,
    TK_ID,    
    TK_PLUS,
    TK_FIELDID,
    TK_FUNID,
    TK_RUID,
    TK_LT,
    TK_LE,
    TK_GT,
    TK_GE,
    TK_EQ,
    TK_NE,
    TK_AND,
    TK_OR,
    TK_NOT,
    TK_ASSIGNOP,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_SQL,
    TK_DOT,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_SQR,
    TK_OP,
    TK_CL,
    TK_EOF,
    TK_ERROR,
    TK_COMMENT,

    /* --- Reserved Words (Keywords) --- */
    TK_WITH,
    TK_PARAMETER,   // Mapped to "parameter"
    TK_PARAMETERS,  // Mapped to "parameters"
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_GLOBAL,
    TK_LIST,
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_ENDWHILE,
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_MAIN,
    TK_EPSILON,        
} TokenType;
static const char *tokenNames[] = {
    /* --- Existing Tokens --- */
    "TK_NUM",        // 0
    "TK_RNUM",       // 1
    "TK_ID",         // 2
    "TK_PLUS",       // 3
    "TK_FIELDID",    // 4
    "TK_FUNID",      // 5
    "TK_RUID",       // 6
    "TK_LT",         // 7
    "TK_LE",         // 8
    "TK_GT",         // 9
    "TK_GE",         // 10
    "TK_EQ",         // 11
    "TK_NE",         // 12
    "TK_AND",        // 13
    "TK_OR",         // 14
    "TK_NOT",        // 15
    "TK_ASSIGNOP",   // 16
    "TK_MINUS",      // 17
    "TK_MUL",        // 18
    "TK_DIV",        // 19
    "TK_SQL",        // 20
    "TK_DOT",        // 21
    "TK_COMMA",      // 22
    "TK_SEM",        // 23
    "TK_COLON",      // 24
    "TK_SQR",        // 25
    "TK_OP",         // 26
    "TK_CL",         // 27
    "TK_EOF",        // 28
    "TK_ERROR",      // 29
    "TK_COMMENT",    // 30

    /* --- Reserved Words  --- */
    "TK_WITH",       // 31
    "TK_PARAMETER",  // 32
    "TK_PARAMETERS", // 33
    "TK_END",        // 34
    "TK_WHILE",      // 35
    "TK_UNION",      // 36
    "TK_ENDUNION",   // 37
    "TK_DEFINETYPE", // 38
    "TK_AS",         // 39
    "TK_TYPE",       // 40
    "TK_GLOBAL",     // 41
    "TK_LIST",       // 42
    "TK_INPUT",      // 43
    "TK_OUTPUT",     // 44
    "TK_INT",        // 45
    "TK_REAL",       // 46
    "TK_ENDWHILE",   // 47
    "TK_IF",         // 48
    "TK_THEN",       // 49
    "TK_ENDIF",      // 50
    "TK_READ",       // 51
    "TK_WRITE",      // 52
    "TK_RETURN",     // 53
    "TK_CALL",       // 54
    "TK_RECORD",     // 55
    "TK_ENDRECORD",  // 56
    "TK_ELSE",       // 57
    "TK_MAIN",        // 58
    "TK_EPSILON",      //59
};

#define B_SIZE 1024
//we will create a structure for twinBuffer with fields array buffer of size 2* B_SIZE, int fp forward pointer and int bp backard pointer and int whichHalf i.e 0 or 1 to indicate which half of the buffer is currently being used
typedef struct {
    char buffer[2 * B_SIZE];
    int fp; // forward pointer
    int bp; // backward pointer
    int whichHalf; // 0 or 1 to indicate which half of the buffer is currently being used
    int line_number; // to keep track of line number for error reporting , preserved accross function calls
} twinBuffer;

//we will create a structure for tokenInfo
typedef struct tokenInfo {
    TokenType type;      // Token Name (e.g., TK_NUM)
    char lexeme[100];     // The actual string (e.g., "12.34")
    int line_number;     // Line number for error reporting
    union {
        int i_val;       // if type == TK_NUM
        double r_val;    // if type == TK_RNUM
    } value;
} tokenInfo;

#define TABLE_SIZE 211

// The Symbol Table Entry
typedef struct SymbolEntry {
    char lexeme[100];         
    TokenType tokenType;
    int line_of_declaration;            
    int dataType;             
    struct SymbolEntry *next; // Pointer for the next item in case of collision
} SymbolEntry;

// The Global Table
extern SymbolEntry* symbolTable[TABLE_SIZE];



#endif