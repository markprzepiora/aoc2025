INCDIRS=-I./include
CFLAGS=-Wall -Wextra -pedantic -std=c2x -ggdb
CFLAGS+=$(INCDIRS)

.PHONY: all bins test clean
all: bins
bins: out/day01a out/day01b out/day02a out/day02b out/day03a out/day03b out/day04a out/day04b out/day05a

out/day01a: day01a.c
	$(CC) $(CFLAGS) $^ -o $@

out/day01b: day01b.c
	$(CC) $(CFLAGS) $^ -o $@

out/day02a: day02a.c
	$(CC) $(CFLAGS) $^ -o $@

out/day02b: day02b.c
	$(CC) $(CFLAGS) $^ -o $@

out/day03a: day03a.c
	$(CC) $(CFLAGS) $^ -o $@

out/day03b: day03b.c include/mrp.c
	$(CC) $(CFLAGS) day03b.c -o out/day03b

out/day04a: day04a.c include/mrp.c
	$(CC) $(CFLAGS) day04a.c -o out/day04a

out/day04b: day04b.c include/mrp.c
	$(CC) $(CFLAGS) day04b.c -o out/day04b

out/day05a: day05a.c include/mrp.c
	$(CC) $(CFLAGS) day05a.c -o out/day05a

clean:
	rm -f *.o out/*

test: bins
	bin/test
