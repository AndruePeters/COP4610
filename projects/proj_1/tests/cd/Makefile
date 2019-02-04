CC=gcc
CFLAGS=-I. -Wall -g -o cd_test.out -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -lglib-2.0

SRC_PATH=../../src/

INC=-I$(SRC_PATH)

all: clean cd_test

cd_test: cd_test.c
	gcc $(CFLAGS) $(INC) cd_test.c

clean:
	- rm *.out *.o
