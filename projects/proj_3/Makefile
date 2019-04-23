CC=gcc
CFLAGS= -Wall -g `pkg-config --cflags  glib-2.0` -I.

src = $(wildcard *.c)

obj = $(src:.c=.o)
LDFLAGS = `pkg-config --libs glib-2.0`



all: clean filesys

filesys: $(obj)
	$(CC)  $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	- rm -f $(obj) filesys
