#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// #define DEBUG
#include "include/mrp.c"

#define BUFFER_SIZE 256
#define JOLTAGE_LENGTH 12
#define DEFAULT_INPUT "input/day03.txt"

#define CTOI(c) ((c) - '0')

static char BUFFER[BUFFER_SIZE];

long int max_joltage(char *buffer, size_t buffer_size)
{
    int digits[JOLTAGE_LENGTH];
    assert(buffer_size >= JOLTAGE_LENGTH && "bank length must be at least JOLTAGE_LENGTH");

    for (size_t digits_index = 0; digits_index < JOLTAGE_LENGTH; digits_index++) {
        size_t digit_index = digits_index > 0 ? digits[digits_index - 1] + 1 : 0;
        for (size_t i = digit_index + 1; i < buffer_size - JOLTAGE_LENGTH + digits_index + 1; i++) {
            int current_max = CTOI(buffer[digit_index]);
            int candidate_max = CTOI(buffer[i]);
            if (candidate_max > current_max) {
                digit_index = i;
            }
        }
        digits[digits_index] = digit_index;
    }

    #ifdef DEBUG
        for (int i = 0; i < JOLTAGE_LENGTH; i++) {
            log_debug("digits[%2d] = %d ... -> %d\n", i, digits[i], CTOI(buffer[digits[i]]));
        }
    #endif

    long int max_joltage = 0;
    for (size_t i = 0; i < JOLTAGE_LENGTH; i++) {
        max_joltage = max_joltage * 10 + CTOI(buffer[digits[i]]);
    }
    return max_joltage;
}

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);

    long int sum_joltage = 0;
    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        assert(is_numeric(BUFFER) && "Encountered non-numeric line in input");
        size_t buffer_size = strlen(BUFFER);
        log_debug("Loaded battery bank: %.*s\n", (int) buffer_size, BUFFER);
        long int joltage = max_joltage(BUFFER, buffer_size);
        log_debug("Joltage: %ld\n", joltage);
        sum_joltage += joltage;
    }

    printf("%ld\n", sum_joltage);

    return 0;
}
