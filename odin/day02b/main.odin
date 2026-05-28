package main

import "core:fmt"
import "core:os"
import "core:strconv"
import "core:text/scanner"

MAX_RANGES_COUNT :: 100
MAX_ID :: 9999999999

Range :: struct {
	start: i64,
	end:   i64,
}

main :: proc() {
	if len(os.args) < 2 {
		fmt.printfln("Usage: %s <input-file>", os.args[0])
		os.exit(2)
	}

	data, err := os.read_entire_file(os.args[1], context.allocator)
	if err != nil {
		fmt.printfln("ERROR: could not read %s: %s", os.args[1], err)
		os.exit(1)
	}
	defer delete(data, context.allocator)

	invalid_ids := make(map[i64]bool)
	defer delete(invalid_ids)

	contents := string(data)
	ranges, ok := parse_ranges(contents)

	sum: i64 = 0
	duplicator_factor: i64 = 10

	for i in 1 ..= 99999 {
		switch i {
		case 10, 100, 1000, 10000:
			duplicator_factor *= 10
		}

		invalid_id := (cast(i64)i * duplicator_factor) + cast(i64)i
		for invalid_id <= MAX_ID {
			for range in ranges {
				if range.start <= invalid_id &&
				   invalid_id <= range.end &&
				   found_invalid(invalid_id, &invalid_ids) {
					sum += invalid_id
				}
			}
			invalid_id = (cast(i64)invalid_id * duplicator_factor) + cast(i64)i
		}
	}

	fmt.printfln("%d", sum)
}

parse_ranges :: proc(contents: string) -> (ranges: [dynamic]Range, ok: bool) {
	s := scanner.Scanner {
		flags = {.Scan_Ints},
	}
	scanner.init(&s, contents)

	for {
		tok := scanner.scan(&s)
		assert(tok == scanner.Int)
		text := scanner.token_text(&s)
		start, start_ok := strconv.parse_i64(text)
		if !start_ok {
			return ranges, false
		}

		tok = scanner.scan(&s)
		assert(tok == '-')

		tok = scanner.scan(&s)
		assert(tok == scanner.Int)
		text = scanner.token_text(&s)
		end, end_ok := strconv.parse_i64(text)
		if !end_ok {
			return ranges, false
		}

		append(&ranges, Range{start, end})

		tok = scanner.scan(&s)
		switch tok {
		case scanner.EOF:
			return ranges, true
		case ',':
			continue
		case:
			return ranges, false
		}
	}
}

parse_error :: proc(s: ^scanner.Scanner, format: string, args: ..any) {
	fmt.eprintf("parse error at %s: ", scanner.position_to_string(s.pos))
	fmt.eprintf(format, ..args)
	fmt.eprintln()
	os.exit(1)
}

found_invalid :: proc(invalid_id: i64, invalid_ids: ^map[i64]bool) -> bool {
	exists := invalid_id in invalid_ids
	if exists {
		return false
	} else {
		invalid_ids[invalid_id] = true
		return true
	}
}
