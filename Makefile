CC=gcc
CFLAGS=-Wall -Werror -pedantic -ggdb -std=c99

objects := $(patsubst %.c,%.o,$(wildcard *.c))

main: $(objects)

.PHONY: clean

clean:
	rm -rf *.o main
