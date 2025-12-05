#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define UNUSED(x) (void)(x)
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

static int cmprange(const Range *r1, const Range *r2)
{
    if (r1->start < r2->start) {
        return -1;
    } else if (r1->start > r2->start) {
        return 1;
    } else {
        return 0;
    }
}

void log_debug_range(Range range) {
    UNUSED(range);
    log_debug("range: %15ld - %15ld\n", range.start, range.end);
}

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);

    log_debug("Parsing ranges\n");
    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        size_t line_len = strlen(BUFFER);
        if (line_len == 0) break;
        long int start;
        long int end;
        assert(sscanf(BUFFER, "%ld-%ld", &start, &end) == 2 && "failed to parse range");
        assert(ranges_count < RANGES_CAP && "too many ranges to parse");
        ranges[ranges_count++] = (Range) { start, end };
        log_debug_range(ranges[ranges_count - 1]);
    }

    qsort(ranges, ranges_count, sizeof(Range), (int (*)(const void *, const void *)) cmprange);

    long int fresh_ingredients_count = 0;

    fresh_ingredients_count += ranges[0].end - ranges[0].start + 1;
    for (size_t i = 1; i < ranges_count; i++) {
        Range *previous_range = &ranges[i-1];
        Range *range = &ranges[i];
        log_debug_range(*range);
        range->start = MAX(previous_range->end + 1, range->start);
        range->end = MAX(previous_range->end, range->end);

        if (range->end >= range->start) {
            fresh_ingredients_count += range->end - range->start + 1;
        }
    }

    printf("%ld\n", fresh_ingredients_count);

    return 0;
}
