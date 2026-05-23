.PHONY: all bins test clean dev
all: bins
bins: \
	out/day01b \
	out/day02b \
	out/day03b

out/%: ./%.odin
	odin build $< -debug -file -out:$@

clean:
	rm -f out/*

test: bins
	bin/test

dev: out/day03b
	out/day03b input/day03example.txt
