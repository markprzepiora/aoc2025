.PHONY: all bins test clean dev
all: bins
bins: \
	out/day01b \
	out/day02b \
	out/day03b \
	out/day04b

out/%: %/main.odin
	odin build $< -debug -file -out:$@

clean:
	rm -f out/*

test: bins
	bin/test

dev: out/day04b
	out/day04b input/day04example.txt
