CC = gcc
## DEPS =

default: memlib.c
	$(CC) -o memlib.a memlib.c -std=c99 -Wall -g

clean:
	-rm -f memlib.a
