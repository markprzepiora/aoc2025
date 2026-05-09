INCDIRS=-I./include
CFLAGS+=-Wall -Wextra -Wconversion -Wsign-conversion -pedantic -std=c2x -lm -ggdb
CFLAGS+=$(INCDIRS)
CC=clang

.PHONY: all bins test clean
all: bins
bins: out/day01a out/day01b \
	out/day02a out/day02b \
	out/day03a out/day03b \
	out/day04a out/day04b \
	out/day05a out/day05b \
	out/day06a out/day06b \
	out/day07a out/day07b \
	out/day08a out/day08b \
	out/day09a out/day09b \
	out/day10a out/day10b \
	out/day11a out/day11b \
	out/day12

out/%: ./%.c include/mrp.c
	$(CC) $(CFLAGS) $< -o $@

out/day10b: ./day10b.c include/mrp.c include/matrix.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o out/*

test: bins
	bin/test
