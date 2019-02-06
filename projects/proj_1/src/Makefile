CC=gcc
CFLAGS= -Wall -g `pkg-config --cflags  glib-2.0` -I.

src = $(wildcard *.c) \
			 $(wildcard builtins/*.c) \
			 $(wildcard utility/*.c)

obj = $(src:.c=.o)
LDFLAGS = `pkg-config --libs glib-2.0`



all: clean my_shell

my_shell: $(obj)
	$(CC)  $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	- rm -f $(obj) my_shell
