package main

import (
	"bufio"
	"bytes"
	"fmt"
	"os"
	"strconv"
	"unicode"
)

var eof = rune(0)

type TokenType int

const (
	TOK_OTHER TokenType = iota
	TOK_DIRECTION
	TOK_NUMBER
	TOK_EOF
)

type Token struct {
	typ TokenType
	val string
}

type scanner struct {
	r *bufio.Reader
}

func main() {
	printHelpAndExit()
	scanner := newScannerFromArgs()

	pos := 50
	password := 0

	for {
		directionToken := scanner.Scan()
		if directionToken.typ == TOK_EOF {
			break
		}
		if directionToken.typ != TOK_DIRECTION {
			fmt.Printf("Expected direction, got: %s\n", directionToken.val)
			os.Exit(1)
		}
		numberToken := scanner.Scan()
		if numberToken.typ != TOK_NUMBER {
			fmt.Printf("Expected number, got: %s\n", numberToken.val)
			os.Exit(1)
		}
		num, _ := strconv.Atoi(numberToken.val)
		if directionToken.val == "L" {
			num = -num
		}

		quot := (pos + num) / 100
		var zero_passes int
		if pos+num <= 0 && pos > 0 {
			zero_passes = abs(quot - 1)
		} else {
			zero_passes = abs(quot)
		}

		password += zero_passes
		pos = (pos + num) % 100
		if pos < 0 {
			pos += 100
		}
	}

	fmt.Printf("%d\n", password)
}

func newScannerFromArgs() *scanner {
	args := os.Args
	f, err := os.Open(args[1])
	if err != nil {
		fmt.Printf("Error opening file: %v\n", err)
		os.Exit(1)
	}
	return newScanner(bufio.NewReader(f))
}

func printHelpAndExit() {
	args := os.Args
	if len(args) == 1 {
		fmt.Printf("Usage: %s <input-file>\n", args[0])
		os.Exit(1)
	}
}

func newScanner(r *bufio.Reader) *scanner {
	return &scanner{r: r}
}

func (s *scanner) read() rune {
	ch, _, err := s.r.ReadRune()
	if err != nil {
		return eof
	}
	return ch
}

func (s *scanner) unread() {
	_ = s.r.UnreadRune()
}

func (s *scanner) scanNumber() Token {
	var buf bytes.Buffer
	ch := s.read()
	for unicode.IsNumber(ch) {
		buf.WriteRune(ch)
		ch = s.read()
	}
	s.unread()
	return Token{typ: TOK_NUMBER, val: buf.String()}
}

func (s *scanner) Scan() Token {
	ch := s.read()

	for unicode.IsSpace(ch) {
		ch = s.read()
	}

	if ch == eof {
		return Token{typ: TOK_EOF, val: ""}
	}

	if ch == 'R' || ch == 'L' {
		return Token{typ: TOK_DIRECTION, val: string(ch)}
	}

	if unicode.IsNumber(ch) {
		s.unread()
		return s.scanNumber()
	}

	return Token{typ: TOK_OTHER, val: string(ch)}
}

func abs(x int) int {
	if x < 0 {
		return -x
	} else {
		return x
	}
}
