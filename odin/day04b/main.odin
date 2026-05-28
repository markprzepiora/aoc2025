package main

import "core:fmt"
import "core:os"

main :: proc() {
	print_help_and_exit()

	data, err := os.read_entire_file_from_path(os.args[1], context.allocator)
}

print_help_and_exit :: proc() {
	if len(os.args) <= 1 {
		fmt.printfln("Usage: %s <input-file>", os.args[0])
		os.exit(1)
	}
}
