CC = gcc
CFLAGS = -Wall -g -DDEBUG --std=c2x
LDFLAGS =
sources = $(wildcard *.c)
objs = $(sources:.c=.o)

all: bfc

bfc: $(objs)
	$(CC) $^ $(LDFLAGS) -o bfc

%.o: %.c
	$(CC) $^ $(CFLAGS) -c -o $@

clean:
	rm $(objs)