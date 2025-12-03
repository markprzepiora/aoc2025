#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// #define DEBUG

#define BUFFER_SIZE 256
static char BUFFER[BUFFER_SIZE];
static char* DEFAULT_INPUT = "input/day03.txt";

long int max_joltage(char *buffer, size_t buffer_size)
{
    int digits[12];

    for (size_t digits_index = 0; digits_index < 12; digits_index++) {
        size_t digit_index = digits_index > 0 ? digits[digits_index - 1] + 1 : 0;
        for (size_t i = digit_index + 1; i < buffer_size - 12 + digits_index + 1; i++) {
            int current_max = buffer[digit_index] - '0';
            int candidate_max = buffer[i] - '0';
            if (candidate_max > current_max) {
                digit_index = i;
            }
        }
        digits[digits_index] = digit_index;
    }

    #ifdef DEBUG
        for (int i = 0; i < 12; i++) {
            printf("digits[%2d] = %d ... -> %d\n", i, digits[i], buffer[digits[i]] - '0');
        }
    #endif

    long int max_joltage = 0;
    for (size_t i = 0; i < 12; i++) {
        max_joltage = max_joltage * 10 + buffer[digits[i]] - '0';
    }
    return max_joltage;
}

int main(int argc, char **argv)
{
    FILE *file = fopen(argc >= 2 ? argv[1] : DEFAULT_INPUT, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    long int sum_joltage = 0;

    while (fgets(BUFFER, BUFFER_SIZE, file)) {
        assert(BUFFER[strlen(BUFFER)-1] == '\n' && "Bank too large to fit in buffer");
        sscanf(BUFFER, "%s", BUFFER);
        size_t buffer_size = strlen(BUFFER);
        #ifdef DEBUG
            printf("Loaded battery bank: %.*s\n", (int) buffer_size, BUFFER);
        #endif
        long int joltage = max_joltage(BUFFER, buffer_size);
        #ifdef DEBUG
            printf("Joltage: %ld\n", joltage);
        #endif
        sum_joltage += joltage;
    }

    printf("%ld\n", sum_joltage);

    return 0;
}
