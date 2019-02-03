CC=gcc
CFLAGS= -Wall -g -o shell.out  -lglib-2.0

SRC_PATH=

INC=-I$(SRC_PATH) -I. -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/lib/x86_64-linux-gnu/glib-2.0/include

all: clean my_shell

my_shell: shell.c
	gcc $(CFLAGS) $(INC) shell.c
	chmod +x shell.out

clean:
	- rm *.out *.o
