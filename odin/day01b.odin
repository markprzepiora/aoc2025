package main

import "core:fmt"
import "core:os"
// import "core:strings"

main :: proc() {
	if len(os.args) < 2 {
		fmt.printfln("Usage: %s <input_file>", os.args[0])
		os.exit(1)
	}
	data, err := os.open(os.args[1])
}
