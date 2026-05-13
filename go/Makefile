.PHONY: all bins test clean
all: bins
bins: \
	out/day01b \
	out/day02b

out/%: ./%/main.go
	go build -o $@ ./$<

clean:
	rm -f out/*

test: bins
	bin/test
