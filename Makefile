INCDIRS=-I./include
CFLAGS=-Wall -Wextra -pedantic -std=c2x -ggdb
CFLAGS+=$(INCDIRS)

.PHONY: all bins test clean
all: bins
bins: out/day01a out/day01b out/day02a out/day02b out/day03a out/day03b \
	out/day04a out/day04b out/day05a out/day05b out/day06a out/day06b \
	out/day07a

out/%: ./%.c include/mrp.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o out/*

test: bins
	bin/test
