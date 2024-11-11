CC=gcc
CFLAGS=-g

make: main.c
	$(CC) $(CFLAGS) -o program main.c

t: test.c
	$(CC) $(CFLAGS) -o program test.c

r:
	clear
	./program tree h1 2>error

clean:
	rm program
