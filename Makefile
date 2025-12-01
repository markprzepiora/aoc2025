INCDIRS=-I./include
CFLAGS=-Wall -Wextra -pedantic -std=c2x -ggdb -fsanitize=undefined
CFLAGS+=$(INCDIRS)

.PHONY: all clean
all: out/day01a

out/day01a: day01a.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o out/*
