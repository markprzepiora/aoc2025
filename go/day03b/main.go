package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

const JOLTAGE_LENGTH = 12

func main() {
	printHelpAndExit()
	data, err := os.ReadFile(os.Args[1])
	if err != nil {
		fmt.Printf("Error reading file: %v\n", err)
		os.Exit(1)
	}
	scanner := bufio.NewScanner(strings.NewReader(string(data)))
	scanner.Split(bufio.ScanWords)
	var sumJoltage int64
	for scanner.Scan() {
		bytes := scanner.Bytes()
		if !isNumeric(bytes) {
			fmt.Printf("Expected numeric input, got: %s\n", scanner.Text())
			os.Exit(1)
		}
		sumJoltage += joltage(bytes)
	}
	fmt.Printf("%d\n", sumJoltage)
}

func printHelpAndExit() {
	if len(os.Args) == 1 {
		fmt.Printf("Usage: %s <input-file>\n", os.Args[0])
		os.Exit(1)
	}
}

func isNumeric(s []byte) bool {
	for _, ch := range s {
		if ch < '0' || ch > '9' {
			return false
		}
	}
	return true
}

func joltage(buffer []byte) int64 {
	// An array of indexes into `buffer`. Once we are done building it, the
	// indexes will be strictly increasing.
	var bufferIndexes [JOLTAGE_LENGTH]int
	for bufferIndexesIndex := range bufferIndexes {
		// start searching at 0 to start, then one index past each last max
		// index
		fromIndex := 0
		if bufferIndexesIndex > 0 {
			fromIndex = bufferIndexes[bufferIndexesIndex-1] + 1
		}
		// how many indexes are there left to find after this one?
		digitsRemaining := JOLTAGE_LENGTH - bufferIndexesIndex - 1
		// don't search too far into the buffer because we need to ensure we
		// have enough room for the remaining digits
		toIndex := len(buffer) - digitsRemaining
		// find the index of the largest digit within the search range and save
		// it
		bufferIndexes[bufferIndexesIndex] =
			fromIndex + maxIndex(buffer[fromIndex:toIndex])
	}

	// compute the integer made up of the subsequence of `buffer` defined by
	// `bufferIndexes`
	var joltage int64
	for _, bufferIndex := range bufferIndexes {
		joltage = joltage*10 + int64(ctoi(buffer[bufferIndex]))
	}
	return joltage
}

func ctoi(char byte) int {
	return int(char - '0')
}

func maxIndex(buffer []byte) int {
	max := 0
	for i := range buffer {
		if buffer[i] > buffer[max] {
			max = i
		}
	}
	return max
}
