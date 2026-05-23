package main

import "core:bufio"
import "core:fmt"
import "core:os"
import "core:strconv"
import "core:strings"

DEBUG :: false

Tok :: struct {
	str: string,
	tok: union #no_nil {
		TokOther,
		TokDirection,
		TokNumber,
		TokEnd,
	},
}

TokDirection :: struct {
	dir: int, // L -> -1, R -> 1
}

TokNumber :: struct {
	num: int,
}

TokEnd :: struct {}

TokOther :: struct {}

next_token :: proc(line: string) -> (tok: Tok, rest: string) {
	if len(line) == 0 {
		return Tok{str = line, tok = TokEnd{}}, line
	}

	if line[0] == 'L' {
		return Tok{str = line[0:1], tok = TokDirection{dir = -1}}, line[1:]
	}

	if line[0] == 'R' {
		return Tok{str = line[0:1], tok = TokDirection{dir = 1}}, line[1:]
	}

	if line[0] >= '0' && line[0] <= '9' {
		length := 1
		for length < len(line) && line[length] >= '0' && line[length] <= '9' {
			length += 1
		}
		num, _ := strconv.parse_int(line[0:length])
		return Tok{str = line[0:length], tok = TokNumber{num = num}}, line[length:]
	}

	return Tok{str = line, tok = TokOther{}}, ""
}

main :: proc() {
	pos := 50
	password := 0

	if len(os.args) < 2 {
		fmt.printfln("Usage: %s <input_file>", os.args[0])
		os.exit(1)
	}

	f, err1 := os.open(os.args[1])
	if err1 != nil {
		fmt.printfln("Error opening file: %s", err1)
		os.exit(1)
	}
	defer os.close(f)

	r: bufio.Reader
	buffer: [1024]byte
	bufio.reader_init_with_buf(&r, os.to_stream(f), buffer[:])
	defer bufio.reader_destroy(&r)

	for {
		n := 0

		line, err := bufio.reader_read_string(&r, '\n')
		if err != nil {
			break
		}
		defer delete(line)
		line = strings.trim_right(line, "\n")

		when DEBUG {
			tokenize_and_print_line(line)
		}

		tok: Tok
		rest := line
		tok, rest = next_token(rest)
		tok_dir, ok1 := tok.tok.(TokDirection)
		if !ok1 {
			fmt.print("Expected a direction token, got: ")
			token_print(tok)
			os.exit(1)
		}
		tok, rest = next_token(rest)
		tok_num, ok2 := tok.tok.(TokNumber)
		if !ok2 {
			fmt.print("Expected a number token, got: ")
			token_print(tok)
			os.exit(1)
		}

		n = tok_num.num * tok_dir.dir
		quot := (pos + n) / 100
		zero_passes := abs(pos + n <= 0 && pos > 0 ? quot - 1 : quot)

		password += zero_passes
		pos = (pos + n) % 100
		if (pos < 0) {
			pos += 100
		}
	}

	fmt.printfln("%d", password)
}

tokenize_and_print_line :: proc(line: string) {
	fmt.printfln("Line: %q", line)
	rest := line
	tok: Tok
	for {
		tok, rest = next_token(rest)
		#partial switch t in tok.tok {
		case TokEnd:
			return
		case:
			fmt.print("    ")
			token_print(tok)
		}
	}
}

token_print :: proc(tok: Tok) {
	switch t in tok.tok {
	case TokDirection:
		fmt.printfln("Tok(%q, TokDirection(dir = %d))", tok.str, t.dir)
	case TokNumber:
		fmt.printfln("Tok(%q, TokNumber(num = %d))", tok.str, t.num)
	case TokOther:
		fmt.printfln("Tok(%q, TokOther())", tok.str)
	case TokEnd:
		fmt.printfln("Tok(%q, TokEnd())", tok.str)
	}
}
