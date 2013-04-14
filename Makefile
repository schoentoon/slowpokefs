CFLAGS := $(CFLAGS) -Wall -O2 -mtune=native -g $(shell pkg-config --cflags fuse)
LFLAGS := $(shell pkg-config --libs fuse) -lpthread
DEFINES:= $(DEFINES) -DVERSION=\""$(shell git describe)"\"
CC     := gcc
BINARY := slowpokefs

.PHONY: all clean dev install

all: slowpokefs

dev: clean
	DEFINES="-DDEV" $(MAKE)

slowpokefs: slowpokefs.c
	$(CC) $(CFLAGS) $(DEFINES) -o $(BINARY) slowpokefs.c $(LFLAGS)

install: slowpokefs
	cp -f $(BINARY) /usr/bin/$(BINARY)

clean:
	rm -rfv $(BINARY)

clang:
	$(MAKE) dev CC=clang
