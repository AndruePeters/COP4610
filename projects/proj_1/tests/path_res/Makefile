CC=gcc
CFLAGS=-I. -Wall -g -o path_test.out

SRC_PATH=../../src/

INC=-I$(SRC_PATH)

all: clean path_test

path_test: path_test.c
	gcc $(CFLAGS) $(INC) path_test.c

clean:
	- rm *.out *.o
