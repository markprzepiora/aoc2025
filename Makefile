INCDIRS=-I./include
CFLAGS=-Wall -Wextra -pedantic -std=c2x -ggdb
CFLAGS+=$(INCDIRS)

.PHONY: all bins test clean
all: bins test
bins: out/day01a out/day01b out/day02a out/day02b out/day03a

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

clean:
	rm -f *.o out/*

test: bins
	bin/test
