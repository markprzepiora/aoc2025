#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// #define DEBUG
#define MAX_RANGES_COUNT 100

static char* DEFAULT_INPUT = "input/day02.txt";

typedef struct Range {
    long int start;
    long int end;
} Range;

int main(int argc, char **argv)
{
    FILE *file = fopen(argc >= 2 ? argv[1] : DEFAULT_INPUT, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    // Load ranges

    Range ranges[MAX_RANGES_COUNT];
    size_t ranges_count = 0;
    long int start;
    long int end;
    while (fscanf(file, "%ld-%ld,", &start, &end) >= 0) {
        assert(ranges_count + 1 < MAX_RANGES_COUNT && "encountered more than max ranges");
        assert(start < 10000000000 && "encountered a 11+ digit long start");
        assert(end < 10000000000 && "encountered a 11+ digit long end");

        #ifdef DEBUG
            printf("Range read: %10lu - %10lu\n", start, end);
        #endif
        ranges[ranges_count++] = ((Range){ start, end });
    }

    // Iterate over all invalid IDs from 11 to 9999999999 and sum the ones
    // represented in the ranges

    long int sum = 0;
    long int duplicator_factor = 10;

    for (int i = 1; i < 99999; i++) {
        switch(i) {
            case 10:
            case 100:
            case 1000:
            case 10000:
                duplicator_factor *= 10;
        }

        long int invalid_id = (i * duplicator_factor) + i;

        for (size_t k = 0; k < ranges_count; k++) {
            Range range = ranges[k];
            if (range.start <= invalid_id && invalid_id <= range.end) {
                #ifdef DEBUG
                    printf("Invalid ID %10lu found in range %10lu - %10lu\n", invalid_id, range.start, range.end);
                #endif
                sum += invalid_id;
            }
        }
    }

    printf("%ld\n", sum);

    return 0;
}
