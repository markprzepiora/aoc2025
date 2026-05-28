package main

import "core:fmt"
import "core:os"
import "core:strings"

JOLTAGE_LENGTH :: 12

main :: proc() {
	print_help_and_exit()

	data, err := os.read_entire_file_from_path(os.args[1], context.allocator)
	if err != nil {
		fmt.printfln("ERROR: could not read input.txt: %s", err)
		os.exit(1)
	}
	defer delete(data, context.allocator)

	str := string(data)
	sum: i64 = 0
	for line in strings.split_lines_iterator(&str) {
		if !is_numeric(line) {
			fmt.printfln("ERROR: invalid line: %s", line)
			os.exit(1)
		}
		sum += joltage(line)
	}
	fmt.printfln("%d", sum)
}

print_help_and_exit :: proc() {
	if len(os.args) <= 1 {
		fmt.printfln("Usage: %s <input-file>", os.args[0])
		os.exit(1)
	}
}

is_numeric :: proc(s: string) -> bool {
	for c in s {
		if c < '0' || c > '9' {
			return false
		}
	}
	return true
}

joltage :: proc(buffer: string) -> i64 {
	buffer_indexes: [JOLTAGE_LENGTH]int
	for _, buffer_indexes_index in buffer_indexes {
		from_index := 0
		if buffer_indexes_index > 0 {
			from_index = buffer_indexes[buffer_indexes_index - 1] + 1
		}
		digits_remaining := JOLTAGE_LENGTH - buffer_indexes_index - 1
		to_index := len(buffer) - digits_remaining
		buffer_indexes[buffer_indexes_index] = from_index + max_index(buffer[from_index:to_index])
	}

	joltage: i64 = 0
	for buffer_index in buffer_indexes {
		joltage = joltage * 10 + cast(i64)(buffer[buffer_index] - '0')
	}
	return joltage
}

max_index :: proc(buffer: string) -> int {
	max := 0
	for i := 1; i < len(buffer); i += 1 {
		if buffer[i] > buffer[max] {
			max = i
		}
	}
	return max
}
