CC = gcc
CFLAGS = -Wall -Werror -O2

.PHONY: all clean run run_queue run_stack run_deque run_all

all: example queue stack deque

example: example.o ringbuf.o
	$(CC) $(CFLAGS) -o $@ $^

queue: data_structure_queue.o ringbuf.o
	$(CC) $(CFLAGS) -o $@ $^

stack: data_structure_stack.o ringbuf.o
	$(CC) $(CFLAGS) -o $@ $^

deque: data_structure_deque.o ringbuf.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c ringbuf.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o example queue stack deque

run: example
	./example

run_queue: queue
	./queue

run_stack: stack
	./stack

run_deque: deque
	./deque

run_all: run run_queue run_stack run_deque

test: run_all
	@echo "\n=== 所有测试完成 ==="
