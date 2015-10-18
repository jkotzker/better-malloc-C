CC = gcc
DEPS = memlib.h

default: mallocme.c
	$(CC) -o memlib.a memlib.h -std=c99 -Wall -g
	$(CC) -o mem mallocme.c -std=c99 -Wall -g

clean:
	-rm -f memlib.a mem
