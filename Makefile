CFLAGS := $(CFLAGS) -Wall -O2 -mtune=native -g $(shell pkg-config --cflags fuse)
LFLAGS := $(shell pkg-config --libs fuse)
DEFINES:= $(DEFINES)
CC     := gcc
BINARY := slowpokefs

.PHONY: all clean dev

all: slowpokefs

dev: clean
	DEFINES="-DDEV" $(MAKE)

slowpokefs: slowpokefs.c
	$(CC) $(CFLAGS) $(DEFINES) -o $(BINARY) slowpokefs.c $(LFLAGS)

clean:
	rm -rfv $(BINARY)

clang:
	$(MAKE) dev CC=clang
