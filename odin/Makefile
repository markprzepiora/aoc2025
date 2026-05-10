.PHONY: all bins test clean
all: bins
bins: out/day01b

out/%: ./%.odin
	odin build $< -debug -file -out:$@

clean:
	rm -f out/*

test: bins
	bin/test
