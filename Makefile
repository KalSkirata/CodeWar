CFLAGS= -Wall -ansi -pedantic -Werror 

all: codewar

codewar: main.o codewar.o
	gcc -o $@ $^
