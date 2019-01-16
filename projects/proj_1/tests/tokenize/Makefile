CC=gcc
CFLAGS=-I. -ansi -pedantic -Wall

SRC_PATH=../../src/

INC=-I$(SRC_PATH)

all: clean tokenize

tokenize: tok_test.c
	gcc $(CFLAGS) $(INC) tok_test.c

clean:
	- rm *.out
