CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -D_GNU_SOURCE

all: myshell

myshell: myshell.c utility.c
	$(CC) $(CFLAGS) myshell.c utility.c -o myshell

clean:
	rm -f myshell

run: myshell
	./myshell

.PHONY: all clean run
