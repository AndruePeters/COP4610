CC=gcc
CFLAGS= -Wall -g `pkg-config --cflags  glib-2.0` -I.

src = $(wildcard *.c)

obj = $(src:.c=.o)
LDFLAGS = `pkg-config --libs glib-2.0`



all: clean filesys

filesys: $(obj)
	$(CC)  $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo -e "\n\nUnpacking and copying image to $(PWD)"
	@tar -xf docs/Project3/fat32.tar.gz

clean:
	- rm -f $(obj) filesys fat32.img
