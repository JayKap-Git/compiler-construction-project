// Group NO.: 3
// Rudra Kaitan 2022B1A70849P
// Bhuvan Arora 2023A7PS0246P
// Prakriti Manocha 2023A7PS0672P
// Anshul Gupta 2023A7PS0587P
// Himanshu Singla 2023A7PS0522P
// Jayant Kapoor 2023A7PS1099P


#include <stdio.h>
#include <stdlib.h>
#include "lexerDef.h"
#include "lexer.h"
#include <string.h>
#include <ctype.h>
#include <math.h>

/*FILE *getStream(FILE *fp, char *bufferSegment, int size): This function takes the input from the file pointed to by 'fp'. 
This file is the source code written in the given language. The function reads the content of the file into a buffer segment of specified size and returns a pointer to the file stream.*/
SymbolEntry* symbolTable[TABLE_SIZE];

void initLexer(FILE *fp, twinBuffer *B) {
    memset(B->buffer, 0, sizeof(B->buffer));
    B->fp = 0;
    B->bp = 0;
    B->whichHalf = 0;
    B->line_number = 1;
    
    // Load the first half of the buffer
    getStream(fp, B->buffer, B_SIZE); 
}

FILE *getStream(FILE *fp, char *bufferSegment, int size) {
    //print checkpoint for debugging
    
    if (fp == NULL) {
        //print file opening error message
        fprintf(stderr, "Error opening file.\n");
        return NULL;
    }

    // fread is used for high-performance block I/O
    size_t bytesRead = fread(bufferSegment, sizeof(char), size, fp);

    // If we read less than the expected size, we've reached the end of the file
    if (bytesRead < size) {
        // Place a sentinel (EOF) at the end of the actual data read
        bufferSegment[bytesRead] = EOF; 
    }
    //print the bytes read for debugging
    
    //returning not requrired but still 
    return fp;
}

//now we will implement the getNextToken function which will take the twinBuffer and file pointer as input and return the next token from the input source code.
tokenInfo getNextToken(twinBuffer *B, FILE *fp) {
    int state=0; // we will use a state variable to keep track of the current state of the DFA
    tokenInfo T;
    int c; // to store the current character being read from the buffer
    T.line_number = B->line_number; // set the line number for the token
    double computedNumber = 0; // to store the computed number from the characters read
    int exponent=0; // to store the exponent part of the number if any
    int sign=1; // to store the sign of the exponent part of the number if any
    //we will store the comment in a separate string variable commentBuffer and we will not return it as a token but we will just ignore it and move on to the next token after the comment
    char commentBuffer[1000]; // to store the comment if any
    int commentIndex = 0; // to keep track of the index in commentBuffer
    // We will read characters until we form a valid token
    while(1){
        
        
        
        if(B->whichHalf==0 && B->fp==B_SIZE){
            //we have to get filld the 2nd half of the buffer
            getStream(fp, B->buffer + B_SIZE, B_SIZE);
            B->whichHalf=1;
        }
        else if(B->whichHalf==1 && B->fp==2*B_SIZE){
            //we need to fill the 1st half of the buffer 
            getStream(fp, B->buffer, B_SIZE);
            B->whichHalf=0;
            B->fp=0; // reset the forward pointer to the start of the buffer
        }
        c = (signed char)B->buffer[B->fp];
        
        
        //we need to see that if c is an unknown character which is not in the language then we will return a token of type TK_ERROR with the lexeme as the unknown character and line number as current line number
        //like if c is $ or any other special character which is not in the language then we will return TK_ERROR token
        //so we will do like this if its not digit, not alphabet , not (comma, dot, colon, semcolon, plus, minus, mul, div, sqr, op, cl) and not white space then we will return TK_ERROR token
        if (!isdigit(c) && !isalpha(c) && c != ',' && c != '.' && c != ':' && c != ';' && c != '+' && c != '-' && c != '*' && c != '/' && c != '[' && c != ']' && c != '(' && c != ')' && !isspace(c)&& !(c == EOF)&& !(c == '~')&& !(c == '#')&& !(c == '<')&& !(c == '>')&& !(c == '=')&& !(c == '!')&& !(c == '&')&& !(c == '@')&& !(c == '%')&& !(c == '_')) {
            //we will return token with type TK_ERROR and lexeme as the unknown character and line number as current line number
            T.type = TK_ERROR;
            if((B->bp - B->fp)!=0){
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    printf("Lexical Error at line %d: incomplete lexeme < '%s' >\n", B->line_number, T.lexeme);
                    return T;

            }
            T.lexeme[0] = c; // set the lexeme to the unknown character
            T.lexeme[1] = '\0'; // null terminate the lexeme string
            T.line_number = B->line_number; // set the line number for the token
            B->fp++; // move forward pointer to read next character in next iteration
            B->bp = B->fp;
            printf("Lexical Error at line %d: Unknown Symbol: < '%s' >\n", B->line_number, T.lexeme);
            return T; // return the error token
        }
        switch(state){
            case 0:{
                //start state
                //from this state the outgoing transitions are upon following characters
                //becase there are many transitions we will use if else if ladder
                // if current char= [0-9] then move to state 1 and compute the number in computedNumber variable
                if (isdigit(c)) {
                    computedNumber = c - '0'; // convert char to int
                    state = 1;  
                    //print the computed number for debugging
                    
                    B->fp++; // move forward pointer to read next character in next iteration
                    break;          
                }
                //if current char = '[' moev to state 46
                else if (c == '[') {
                    state = 46;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break;
                } 
                //if current char = ']' move to state 47
                else if (c == ']') {
                    state = 47;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if current char='(' move to state 48
                else if (c == '(') {
                    state = 48;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if current char=')' move to state 49
                else if (c == ')') {
                    state = 49;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break;
                }
                //, if current char=',' move to state 50
                else if (c == ',') {
                    state = 50;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //: if current char=':' move to state 51
                else if (c == ':') {
                    state = 51;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break;
                }
                //; if current char=';' move to state 52
                else if (c == ';') {
                    state = 52;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //. if current char='.' move to state 53
                else if (c == '.') {
                    state = 53;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //+ if current char='+' move to state 54
                else if (c == '+') {
                    state = 54;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //- if current char='-' move to state 55
                else if (c == '-') {
                    state = 55;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //* if current char='*' move to state 56
                else if (c == '*') {
                    state = 56;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if current char='/' move to state 57
                else if (c == '/') {
                    state = 57;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //~ if current char='~' move to state 58
                else if (c == '~') {
                    state = 58;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if current char is white space space tab newline stay in the state 0 only , but in case of newline increment the line number counter
                else if (isspace(c)) {
                    if (c == '\n') {
                        //increment line number counter
                        B->line_number++;
                    }
                    B->fp++; // move forward pointer to read next character in next iteration
                    B->bp=B->fp;
                    break; 
                }
                //if [b-d] move to state 13
                else if (c >= 'b' && c <= 'd') {
                    state = 13;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if [a-z]-[b-d] move to state 11
                else if ((c >= 'a' && c <= 'z') && !(c >= 'b' && c <= 'd')) {
                    state = 11;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if _ move to state 17
                else if (c == '_') {
                    state = 17;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //# move to 21
                else if (c == '#') {
                    state = 21;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //< 24
                else if (c == '<') {
                    state = 24;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //> 31
                else if (c == '>') {
                    state = 31;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //= 34
                else if (c == '=') {
                    state = 34;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //! 36
                else if (c == '!') {
                    state = 36;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //& 38
                else if (c == '&') {
                    state = 38;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //@ 41
                else if (c == '@') {
                    state = 41;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //% 45
                else if (c == '%') {
                    
                    
                    state = 45;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; 
                }
                //if EOF then return TK_EOF token
                else if (c == EOF) {
                    
                    //fo to state 61 where we will return TK_EOF token
                    state = 61;
                    break;
                }
                else{
                    T.type = TK_ERROR;
                    T.lexeme[0] = c; // set the lexeme to the unknown character
                    T.lexeme[1] = '\0'; // null terminate the lexeme string
                    T.line_number = B->line_number; // set the line number for the token
                    B->fp++; // move forward pointer to read next character in next iteration
                    B->bp = B->fp;
                    printf("Lexical Error at line %d: Unknown Symbol: < '%s' >\n", B->line_number, T.lexeme);
                    return T; // return the error token
                }
                break;

            }
            case 1:{
                //from this state we have following transitions
                //if current char is [0-9] then stay in the state and compute the number in computedNumber variable
                if (isdigit(c)) {
                   
                    computedNumber = computedNumber * 10 + (c - '0'); // convert char to int and add to computedNumber
                   
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //U is the set of all ascii charaters , if we get U-{[0-9],.} then move to stat 9 
                else if (c != '.' && !isdigit(c)) {
                  
                    state = 9;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
                //if current char is '.' move to state 2
                else if (c == '.') {
                    state = 2;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
            }
            case 2:{
                //in this state we read 1 of 2 decimal places of the number
                //if [0-9] then compute the number in computedNumber variable and move to state 3 for 2nd decimal place
                if (isdigit(c)) {
                    computedNumber = computedNumber + (c - '0') / 10.0; // convert char to int and add to computedNumber
                    state = 3;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if U-{[0-9]} then print the full current lexeme from bp to fp as error with the line number and increment fp and move to state 0 in hope to futher tokenise
                else {
                    
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    printf("Lexical Error at line %d: Invalid number format: 2 digits expected after decimal: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token

                }
            }
            case 3:{
                //here we read the 2nd decimal place of the number
                //if [0-9] then compute the number in computedNumber variable and move to state 4
                if (isdigit(c)) {
                    computedNumber = computedNumber + (c - '0') / 100.0; // convert char to int and add to computedNumber
                    state = 4;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if U-{[0-9]} then print the full current lexeme from bp to fp as error with the line number and increment fp and move to state 0 in hope to futher tokenise
                else {
                    
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    printf("Lexical Error at line %d: Invalid number format: 2 digits expected after decimal: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token

                }
            }
            case 4:{
                //in this state we have read the full number with 2 decimal places
                //if U-{E}then we will go to state 10
                if (c != 'E') {
                    state = 10;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
                //if current char is 'E' then we will move to state 5 to read the exponent part of the number
                else if (c == 'E') {
                    state = 5;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }

            }
            case 5:{
                // in this state we either read + or - or directly the exponent digits 
                //if current char is '+ or -' move to state 6 but store the sign of the exponent in a variable sign
                if (c == '+') {
                    sign = 1;
                    state = 6;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                else if (c == '-') {
                    sign = -1;
                    state = 6;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if current char is [0-9] then compute the exponent value in exponent variable sign = +1 and move to state 7
                else if (isdigit(c)) {
                    exponent = (c - '0'); // compute exponent value
                    sign = 1;
                    state = 7;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[0-9],+,-} then this is an error because we are expecting exponent part after 'E' , in such case we will print error with the full lexeme from bp to fp and the line number and move to state 0 after incrementing fp
                else {
                    
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    printf("Lexical Error at line %d: Invalid number format: +,- or digit expected after E: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token

                }

            }
            case 6:{
                //in this state we have read the sign of the exponent
                //if current char is [0-9] then compute the exponent value in exponent variable and move to state 7
                if (isdigit(c)) {
                    exponent = exponent * 10 + (c - '0'); // compute exponent value
                    state = 7;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if U-{[0-9]} then this is an error because we are expecting exponent digits after the sign of exponent , in such case we will print error with the full lexeme from bp to fp and the line number and move to state 0 after incrementing fp
                else {
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    printf("Lexical Error at line %d: Invalid number format: 2 exponent digits expected after sign: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    return T; // return the error token
                }
            }
            case 7:{
                //in this state we have read 1 exponent digit we need to consume 1 more exponent digit and move to state 8
                //if current char is [0-9] then compute the exponent value in exponent variable and move to state 8
                if (isdigit(c)) {
                    exponent = exponent * 10 + (c - '0'); // compute exponent value
                    state = 8;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[0-9]} then this is an error because we are expecting exponent digits , in such case we will print error with the full lexeme from bp to fp and the line number and move to state 0 after incrementing fp
                else {
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid number format: 2 exponent digits expected but got 1: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    return T; // return the error token
                }
            }
            case 8:{
                //in this state we have read the full number with exponent part
                
                
                
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    //tokenise the lexeme from bp to fp as TK_RNUM with the value computed in computedNumber variable and exponent value in exponent variable and line number in lineNumber variable
                    //after tokenising we need to make bp=fp and move to state 0 for next token
                    //we already have created token variable T to store the token information we will use that variable to store the token information and return it at the end of this case
                    
                    T.type = TK_RNUM;
                    //we need to pay special attention in the case when bp>fp becasue of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                   //now we can do rest of return formalities for the token
                    T.line_number = B->line_number;
                    //we need to compute the final value of the number using computedNumber variable and exponent value in exponent variable and sign of exponent in sign variable
                    T.value.r_val = computedNumber * pow(10, sign * exponent); // compute the final value of the number
                    B->bp = B->fp; // move bp to fp to start reading next token
                    return T; // return the token
                
            }
            case 9:{
                // this is the state where we have read a full integer number without any decimal or exponent part
                //we came here becues  we read U-{[0-9]}
                //we will tokenise the lexeme from bp to fp as TK_NUM with the value computed in computedNumber variable and line number in lineNumber variable
                //we will also need to move bp to fp to start reading next token and move to state 0 for next token
                //similarly here also , bacically wehere ever we are tokenising a lexeme we need to take care of the case when bp>fp because of cyclic nature of twin buffer and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                T.type = TK_NUM;
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                T.value.i_val = (int)computedNumber; // store the integer value of the number in token value
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token   
                
            }
            
            case 10:{
                //in this state we have read the full number with 2 decimal places but no exponent part
                //we came here because we read U-{[0-9],E]}
                //we will Tokenies this lexeme as TK_RNUM with the value computed in computedNumber variable and line number in lineNumber variable

                T.type = TK_RNUM;
                //we need to pay special attention in the case when bp>fp 
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                T.value.r_val = computedNumber; // store the value of the number in token value
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 11:{
                //in this state we have read the first character of an identifier which is in [a-z]-[b-d]
                //if current char is [a-z] then stay in the state and keep reading the identifier
                if ((c >= 'a' && c <= 'z')) {
                    state = 11;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if U-{[a-z]} then move to state 12
                else {
                    state = 12;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 12:{
                //we have read full feld identifier in this state
                //we came here because we read U-{[a-z]} after reading some characters in [a-z] which is a valid identifier
                //we will tokenise the lexeme from bp to fp as TK_ID with the value of lexeme and line number in lineNumber variable
                T.type = TK_FIELDID;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token

                SymbolEntry *entry = lookup(T.lexeme);
                if (entry != NULL) {
                    T.type = entry->tokenType; 
                } else {
                    // 3. New variable! Push it into the table
                    insert(T.lexeme, T.type); 
                }   

                return T; // return the token
            }
            case 13:{
                //if [a-z] move to state 11
                if ((c >= 'a' && c <= 'z')) {
                    state = 11;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if [2-7] move to state 14
                else if (c >= '2' && c <= '7') {
                    state = 14;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[a-z],2-7} move to state 12
                else {
                    state = 12;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 14:{
                //if [b-d] stay in 14
                if (c >= 'b' && c <= 'd') {
                    state = 14;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if [2-7] move to state 15
                else if (c >= '2' && c <= '7') {
                    state = 15;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[b-d],2-7} move to state 16
                else {
                    state = 16;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 15:{
                //if [2-7] stay in 15
                if (c >= '2' && c <= '7') {
                    state = 15;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[2-7]} move to state 16
                else {
                    state = 16;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 16:{
                // in this state we haev read full identifier
                //now we will tokenise the lexeme from bp to fp as TK_ID with the value of lexeme and line number in lineNumber variable
                T.type = TK_ID;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                //if length of lexeme >20 then throw error because maximum length of identifier is 20
                if (strlen(T.lexeme) > 20) {
                    printf("Lexical Error at line %d: Identifier '%s' exceeds maximum length of 20 characters\n", B->line_number, T.lexeme);
                    T.type = TK_ERROR; // change token type to error
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                SymbolEntry *entry = lookup(T.lexeme);
                if (entry != NULL) {
                    // 2. Found it! Use the type from the table (Keyword resolution)
                    
                    T.type = entry->tokenType; 
                } else {
                    // 3. New variable! Push it into the table
                    insert(T.lexeme, T.type); 
                }   

                return T; // return the token

            }
            case 17:{
                // we have read first character of an TK_FUNID which is _
                //if [a-z|A_Z]
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    state = 18;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[a-z|A-Z]} then this is an error because we are expecting function identifier after _ , in such case we will print error with the full lexeme from bp to fp and the line number and move to state 0 after incrementing fp
                else {
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid Function ID format: expected format is [a-z|A-Z]: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    SymbolEntry *entry = lookup(T.lexeme);
                    if (entry != NULL) {
                        // 2. Found it! Use the type from the table (Keyword resolution)
                        
                        T.type = entry->tokenType; 
                    } else {
                        // 3. New variable! Push it into the table
                        insert(T.lexeme, T.type); 
                    }   
                    return T; // return the error token
                }
            }
            case 18:{
                //in this state we have read the first character of a function identifier which is in [a-z|A-Z] and the first character is _ 
                //if current char is [a-z|A-Z] then stay in the state and keep reading the function identifier
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    state = 18;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if [0-9] move to state 19
                else if (isdigit(c)) {
                    state = 19;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[a-z|A-Z],0-9} then move to state 20
                else {
                    state = 20;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break;
                }

            }
            case 19:{
                //if current char is [0-9] then stay in the state and keep reading the function identifier
                if (isdigit(c)) {
                    state = 19;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character         
                }
                //if U-{[0-9]} then move to state 20
                else {
                    state = 20;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break;
                }
            }
            case 20:{
                T.type = TK_FUNID;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                //if length of lexeme >30 then throw error because maximum length of identifier is 30
                if (strlen(T.lexeme) > 30) {
                    printf("Lexical Error at line %d: Identifier '%s' exceeds maximum length of 30 characters\n", B->line_number, T.lexeme);
                    T.type = TK_ERROR; // change token type to error
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                SymbolEntry *entry = lookup(T.lexeme);
                if (entry != NULL) {
                    // 2. Found it! Use the type from the table (Keyword resolution)
                    
                    T.type = entry->tokenType; 
                } else {
                    // 3. New variable! Push it into the table
                    insert(T.lexeme, T.type); 
                }   
                return T; // return the token
            }
            case 21:{
                //we have already read # i.e begin of TK_RUID record and union identifier
                //if [a-z] move to state 22
                if (c >= 'a' && c <= 'z') {
                    state = 22;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[a-z]} then this is an error because we are expecting record and union identifier after # , in such case we will print error with the full lexeme from bp to fp and the line number and move to state 0 after incrementing fp
                else {
                    //we will return token with type TK_ERROR and lexeme as the full current lexeme from bp to fp-1 and line number as current line number
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid Record/Union ID format: expected format [a-z]: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    SymbolEntry *entry = lookup(T.lexeme);
                    if (entry != NULL) {
                        // 2. Found it! Use the type from the table (Keyword resolution)
                        
                        T.type = entry->tokenType; 
                    } else {
                        // 3. New variable! Push it into the table
                        insert(T.lexeme, T.type); 
                    }   
                    return T; // return the error token
                }
            }
            case 22:{
                //if [a-z] stay in state 22
                if (c >= 'a' && c <= 'z') {
                    state = 22;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{[a-z]} then move to state 23
                else {
                    state = 23;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 23:{
                T.type = TK_RUID;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                //if bp<fp then we can directly copy the lexeme from bp to fp-1
                else {
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                SymbolEntry *entry = lookup(T.lexeme);
                if (entry != NULL) {
                    // 2. Found it! Use the type from the table (Keyword resolution)
                    
                    T.type = entry->tokenType; 
                } else {
                    // 3. New variable! Push it into the table
                    insert(T.lexeme, T.type); 
                }   
                return T; // return the token
            }
            case 24:{
                //we have 3 things here < <= and <--- which are TK_LT TK_LE and TK_ASSIGNOP respectively
                //if current char is '=' then this is TK_LE and we move to state 29
                if (c == '=') {
                    state = 29;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if current char is '-' then this is TK_ASSIGNOP and we move to state 25
                else if (c == '-') {
                    state = 25;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{=,-} then move to state 28
                else {
                    state = 28;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 25:{
                //in this state we have read <- which is TK_ASSIGNOP
                //if current char is '-' then move to state 26
                if (c == '-') {
                    state = 26;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{-} then its an error because we are expecting <--- for assignment operator but we got <- 
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid Assignment operator format: expected format is <---: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    return T; // return the error token
                }
            }
            case 26:{
                //we haev already read <-- 
                // now if - comes then moveto state 27
                if (c == '-') {
                    state = 27;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{-} then its an error because we are expecting <--- for assignment operator but we got <-- 
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid Assignment operator format: expected format is <---: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;   
                    B->bp = B->fp;
                    return T; // return the error token
                }
            }
            case 27:{
                T.type = TK_ASSIGNOP;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 28:{
                T.type = TK_LT;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 29:  {
                T.type = TK_LE;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 31:{
                //in these states we handel the cases for > and >= 
                //if = then move to state 33
                if (c == '=') {
                    state = 33;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{=} then move to state 32
                else {
                    state = 32;
                    // we will not move fp , becasue this is wehre we will resume next lexeme
                    break; 
                }
            }
            case 32:{
                T.type = TK_GT;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 33:{
                T.type = TK_GE;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 34:{
                //in this state we handel the case for ==
                //if = then move to state 35
                if (c == '=') {
                    state = 35;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{=} then error
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid equality check operator format: expected format is ==: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp; // move bp to fp to start reading next token
                    return T; // return the error token
                }                
            }
            case 35:{
                T.type = TK_EQ;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 36:{
                //in this state we handel the case for !=
                //if = then move to state 37
                if (c == '=') {
                    state = 37;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{=} then error
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid inequality check operator format: expected format is !=: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token
                }
            }
            case 37:{
                T.type = TK_NE;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 38:{
                //in this state we handel the case for &&
                //if & then move to state 39
                if (c == '&') {
                    state = 39;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{&} then error
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid And operator format: expected format is &&&: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token
                }
            }
            case 39:{
                //in this state we have read && 
                //need to read 1 more & to make it &&& which is TK_AND
                if (c == '&') {
                    state = 40;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{&} then error because we are expecting &&& for logical AND operator but we got && 
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid And operator format: expected format is &&&: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token
                }
            }
            case 40:{
                T.type = TK_AND;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 41:{
                // here we handel @@@ which is TK_OR
                //already read @ 
                //if @ then move to state 42
                if (c == '@') {
                    state = 42;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                //if U-{@} then error because we are expecting @@@ for logical OR operator but we got @ 
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid OR operator format: expected format is @@@: < '%s' >\n", B->line_number, T.lexeme);
                    T.line_number = B->line_number;
                    B->bp = B->fp;
                    return T; // return the error token
                }
            }
            case 42:{
                //in this state we have read @@ 
                //need to read 1 more @ to make it @@@ which is TK_OR
                if (c == '@') {
                    state = 43;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break;; // continue to next iteration to read next character
                }
                //if U-{@} then error because we are expecting @@@ for logical OR operator but we got @@ 
                else {
                    T.type = TK_ERROR;
                    //we need to pay special attention in the case when bp>fp because of cyclic nature of twin array and in such case we need to copy the lexeme in two parts one from bp to end of buffer and other from start of buffer to fp-1
                    if (B->bp > B->fp) {
                        //copy the lexeme in two parts
                        int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                        int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                        strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                        T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                    }
                    else {
                        //if bp<fp then we can directly copy the lexeme from bp to fp-1
                        strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                        T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                    }
                    //print the lexeme and line number in error message
                    printf("Lexical Error at line %d: Invalid OR operator format: expected format is @@@: < '%s' >\n", B->line_number, T.lexeme);
                    B->bp = B->fp;
                    T.line_number = B->line_number;
                    return T; // return the error token
                }
            }
            case 43:{
                T.type = TK_OR;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 45:{
                //this is the state for handeling comments
                //we will read characters until we encounter a newline character which indicates the end of comment
                //we will also increment the line number after new line character
                if (c != '\n') {
                    state=45;
                    //store the commetn in the commetn variable
                    commentBuffer[commentIndex++] = c;
                    B->fp++; // move forward pointer to read next character in next iteration
                    break; // continue to next iteration to read next character
                }
                else {
                    //print the commetn buffer
                    commentBuffer[commentIndex] = '\0'; // null terminate the comment string
                    //reset the comment buffer and index for next comment
                    commentIndex = 0;
                    //memeset the commetn bufder
                    memset(commentBuffer, 0, sizeof(commentBuffer));
                    //B->line_number++; // increment line number after reading a new line character

                    //B->fp++; // move forward pointer to read next character in next iteration
                    //bring bp to fp to start reading next token after comment
                    B->bp = B->fp;
                    //we will return the tokem TK_COMMENT because this is the end of comment
                    T.type=TK_COMMENT;
                    //we will return the lexeme as just %
                    T.lexeme[0]='%';
                    T.lexeme[1]='\0';
                    T.line_number=B->line_number;
                    return T;
                    
                }
                
            }
            case 46:{
                T.type = TK_SQL;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 47:{
                T.type = TK_SQR;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 48:{
                T.type = TK_OP;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 49:{
                T.type = TK_CL;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 50:{
                T.type = TK_COMMA;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 51:{
                T.type = TK_COLON;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 52:{
                T.type = TK_SEM;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 53:{
                T.type = TK_DOT;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 54:{
                T.type = TK_PLUS;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 55:{
                T.type = TK_MINUS;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 56:{
                T.type = TK_MUL;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 57:{
                T.type = TK_DIV;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            case 58:{
                T.type = TK_NOT;
                //we need to pay special attention in the case when bp>fp because of cyclic nature
                if (B->bp > B->fp) {
                    //copy the lexeme in two parts
                    int firstPartLength = 2*B_SIZE - B->bp; // length of first part from bp to end of buffer
                    int secondPartLength = B->fp; // length of second part from start of buffer to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, firstPartLength); // copy first part to token lexeme
                    strncpy(T.lexeme + firstPartLength, B->buffer, secondPartLength); // copy second part to token lexeme
                    T.lexeme[firstPartLength + secondPartLength] = '\0'; // null terminate the lexeme string
                }
                else {
                    //if bp<fp then we can directly copy the lexeme from bp to fp-1
                    strncpy(T.lexeme, B->buffer + B->bp, B->fp - B->bp); // copy lexeme from bp to fp-1 to token lexeme
                    T.lexeme[B->fp - B->bp] = '\0'; // null terminate the lexeme string
                }
                T.line_number = B->line_number;
                B->bp = B->fp; // move bp to fp to start reading next token
                return T; // return the token
            }
            
            case 61:{
                //we haev encountered and EOF character thats whyyw e are in this state and we will return a token of type TK_EOF with lexeme "EOF" and line number as current line number in buffer
                
                T.type = TK_EOF;
                strcpy(T.lexeme, "EOF");
                T.line_number = B->line_number;
                return T;
            }


        }

    }

}

//now we will implemet remove comments function
void removeComments(char *testcaseFile, char *cleanFile){
    //we will open both the files
    FILE *fp = fopen(testcaseFile, "r");
    if (fp == NULL) {
        perror("Error opening source file");
        return ;
    }

    // 2. Open (or create) the output file for writing
    FILE *fout = fopen(cleanFile, "w");
    if (fout == NULL) {
        perror("Error creating output file");
        fclose(fp);
        return ;
    }
    //we will create a buffer to take input from the test case file. 
    //Here twin buffer is not required, single buffer will suffice because we are just removing comments. 
    char buffer[B_SIZE];
    size_t bytesRead;
    int inComment = 0; // State flag: 0 = Code, 1 = Inside a % comment

    // Read the file chunk by chunk
    while ((bytesRead = fread(buffer, 1, B_SIZE, fp)) > 0) {
        for (size_t i = 0; i < bytesRead; i++) {
            char c = buffer[i];

            if (inComment) {
                // If we are inside a comment, wait for a newline to exit
                if (c == '\n') {
                    inComment = 0;
                    fputc('\n', fout); // Keep the newline for line numbering
                }
            } else {
                // If we aren't in a comment, check for the start of one
                if (c == '%') {
                    inComment = 1;
                } else {
                    fputc(c, fout); // Valid code character
                }
            }
        }
    }

    
    

}
unsigned int hash(char *str) {
    unsigned int hash = 5381;
    int c;
    while ((c = *str++)) {
        // hash * 33 + c (the bit shift is faster than multiplication)
        hash = ((hash << 5) + hash) + c; 
    }
    return hash % TABLE_SIZE;
}
// LOOKUP: Returns the entry if found, otherwise returns NULL
SymbolEntry* lookup(char *lexeme) {
    unsigned int slot = hash(lexeme);
    SymbolEntry *current = symbolTable[slot];

    while (current != NULL) {
        if (strcmp(current->lexeme, lexeme) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
// INSERT: Creates a new entry and handles the collision chaining
SymbolEntry* insert(char *lexeme, TokenType tokenType) {
    unsigned int slot = hash(lexeme);

    // Create a new node in memory
    SymbolEntry *newEntry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
    if (newEntry == NULL) {
        fprintf(stderr, "Fatal Error: Out of memory for Symbol Table\n");
        exit(1);
    }

    // Fill the node
    strncpy(newEntry->lexeme, lexeme, 99);
    newEntry->lexeme[99] = '\0'; // Ensure null-termination
    newEntry->tokenType = tokenType;
    newEntry->dataType = -1; // Initialize as unknown for the Parser

    // Collision handling: Insert at the head of the list for this slot
    newEntry->next = symbolTable[slot];
    symbolTable[slot] = newEntry;

    return newEntry;
}
void initSymbolTable() {
    // 1. Clear the table initially
    for (int i = 0; i < TABLE_SIZE; i++) {
        symbolTable[i] = NULL;
    }

    // 2. Populate with reserved words
    insert("with", TK_WITH);
    
    insert("parameter", TK_PARAMETER); 
    insert("parameters", TK_PARAMETERS);
    insert("end", TK_END);
    insert("while", TK_WHILE);
    insert("union", TK_UNION);
    insert("endunion", TK_ENDUNION);
    insert("definetype", TK_DEFINETYPE);
    insert("as", TK_AS);
    insert("type", TK_TYPE);
    insert("global", TK_GLOBAL);
    insert("list", TK_LIST);
    insert("input", TK_INPUT);
    insert("output", TK_OUTPUT);
    insert("int", TK_INT);
    insert("real", TK_REAL);
    insert("endwhile", TK_ENDWHILE);
    insert("if", TK_IF);
    insert("then", TK_THEN);
    insert("endif", TK_ENDIF);
    insert("read", TK_READ);
    insert("write", TK_WRITE);
    insert("return", TK_RETURN);
    insert("call", TK_CALL);
    insert("record", TK_RECORD);
    insert("endrecord", TK_ENDRECORD);
    insert("else", TK_ELSE);
    insert("_main", TK_MAIN);
}



