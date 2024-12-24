CC = gcc
CFLAGS = -Wall -Werror -O2

all: example

example: example.o ringbuf.o
	$(CC) $(CFLAGS) -o example example.o ringbuf.o

example.o: example.c ringbuf.h
	$(CC) $(CFLAGS) -c example.c

ringbuf.o: ringbuf.c ringbuf.h
	$(CC) $(CFLAGS) -c ringbuf.c

clean:
	rm -f *.o example

run: example
	./example
