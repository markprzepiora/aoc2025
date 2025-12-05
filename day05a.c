#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// #define DEBUG
#include "include/mrp.c"

#define BUFFER_SIZE 256
#define RANGES_CAP 200
#define DEFAULT_INPUT "input/day05example.txt"
static char BUFFER[BUFFER_SIZE];

typedef struct Range {
    long int start;
    long int end;
} Range;

static Range ranges[RANGES_CAP] = { 0 };
static size_t ranges_count = 0;

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);

    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        size_t line_len = strlen(BUFFER);
        if (line_len == 0) break;
        long int start;
        long int end;
        assert(sscanf(BUFFER, "%ld-%ld", &start, &end) == 2 && "failed to parse range");
        assert(ranges_count < RANGES_CAP && "too many ranges to parse");
        ranges[ranges_count++] = (Range) { start, end };
        log_debug("parsed range: %ld - %ld\n", start, end);
    }

    int fresh_ingredients_count = 0;

    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        long int id;
        assert(sscanf(BUFFER, "%ld", &id) == 1 && "failed to parse ingredient ID");
        log_debug("parsed ID: %ld\n", id);
        for (size_t i = 0; i < ranges_count; i++) {
            Range range = ranges[i];
            if (range.start <= id && id <= range.end) {
                fresh_ingredients_count++;
                break;
            }
        }
    }

    printf("%d\n", fresh_ingredients_count);

    return 0;
}
