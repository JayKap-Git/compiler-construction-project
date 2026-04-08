# Group NO.: 3
# Rudra Kaitan 2022B1A70849P
# Bhuvan Arora 2023A7PS0246P
# Prakriti Manocha 2023A7PS0672P
# Anshul Gupta 2023A7PS0587P
# Himanshu Singla 2023A7PS0522P
# Jayant Kapoor 2023A7PS1099P


CC     = gcc
CFLAGS = -Wall -g
LIBS   = -lm

OBJS = driver.o lexer.o parser.o

stage1exe: $(OBJS)
	$(CC) $(CFLAGS) -o stage1exe $(OBJS) $(LIBS)

driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c driver.c

parser.o: parser.c parser.h parserDef.h lexer.h
	$(CC) $(CFLAGS) -c parser.c

lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c lexer.c

clean:
	rm -f stage1exe $(OBJS) output.txt
