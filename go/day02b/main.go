package main

import (
	"bufio"
	"fmt"
	"os"
)

var MAX_ID int64 = 9999999999

type Interval struct {
	Start int64
	End   int64
}

func main() {
	printHelpAndExit()
	reader := readerFromArgs()

	invalidIDs := make(map[int64]bool)
	intervals := make([]Interval, 0)

	var start int64
	var end int64
	for {
		_, err := fmt.Fscanf(reader, "%d-%d", &start, &end)
		if err != nil {
			break
		}
		if start >= 10000000000 {
			fmt.Fprintf(os.Stderr, "ERROR: encountered a 11+ digit long start: %d\n", start)
			os.Exit(1)
		}
		if end >= 10000000000 {
			fmt.Fprintf(os.Stderr, "ERROR: encountered a 11+ digit long end: %d\n", end)
			os.Exit(1)
		}
		intervals = append(intervals, Interval{Start: start, End: end})
		_, err = fmt.Fscanf(reader, ",")
		if err != nil {
			break
		}
	}

	var sum int64 = 0
	var duplicatorFactor int64 = 10
	var i int64

	for i = 1; i <= 99999; i++ {
		switch i {
		case 10, 100, 1000, 10000:
			duplicatorFactor *= 10
		}
		invalidID := (i * duplicatorFactor) + i
		for invalidID <= MAX_ID {
			for _, interval := range intervals {
				if interval.Start <= invalidID &&
					invalidID <= interval.End &&
					foundInvalid(invalidID, invalidIDs) {
					sum += invalidID
				}
			}
			invalidID = (invalidID * duplicatorFactor) + i
		}
	}

	fmt.Printf("%d\n", sum)
}

func printHelpAndExit() {
	if len(os.Args) == 1 {
		fmt.Printf("Usage: %s <input-file>\n", os.Args[0])
		os.Exit(1)
	}
}

func readerFromArgs() *bufio.Reader {
	args := os.Args
	f, err := os.Open(args[1])
	if err != nil {
		fmt.Printf("Error opening file: %v\n", err)
		os.Exit(1)
	}
	return bufio.NewReader(f)
}

func foundInvalid(invalidID int64, invalidIDs map[int64]bool) bool {
	exists := invalidIDs[invalidID]
	if exists {
		return false
	} else {
		invalidIDs[invalidID] = true
		return true
	}
}
