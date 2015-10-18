CC = gcc
## DEPS =

default: memlib.h
	$(CC) -o memlib.a memlib.h -std=c99 -Wall -g

clean:
	-rm -f memlib.a
