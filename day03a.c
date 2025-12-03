#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// #define DEBUG

#define BUFFER_SIZE 256
static char BUFFER[BUFFER_SIZE];

int max_joltage(char *buffer, size_t buffer_size)
{
    size_t first_digit_index = 0;
    for (size_t i = first_digit_index + 1; i < buffer_size - 1; i++) {
        int current_max = buffer[first_digit_index] - '0';
        int candidate_max = buffer[i] - '0';
        if (candidate_max > current_max) {
            first_digit_index = i;
        }
    }
    size_t second_digit_index = first_digit_index + 1;
    for (size_t i = second_digit_index + 1; i < buffer_size; i++) {
        int current_max = buffer[second_digit_index] - '0';
        int candidate_max = buffer[i] - '0';
        if (candidate_max > current_max) {
            second_digit_index = i;
        }
    }
    return (buffer[first_digit_index] - '0') * 10 + (buffer[second_digit_index] - '0');
}

static char* DEFAULT_INPUT = "input/day03.txt";
int main(int argc, char **argv)
{
    FILE *file = fopen(argc >= 2 ? argv[1] : DEFAULT_INPUT, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    int sum_joltage = 0;

    while (fgets(BUFFER, BUFFER_SIZE, file)) {
        assert(BUFFER[strlen(BUFFER)-1] == '\n' && "Bank too large to fit in buffer");
        sscanf(BUFFER, "%s", BUFFER);
        size_t buffer_size = strlen(BUFFER);
        #ifdef DEBUG
            printf("Loaded battery bank: %.*s\n", (int) buffer_size, BUFFER);
        #endif
        int joltage = max_joltage(BUFFER, buffer_size);
        #ifdef DEBUG
            printf("Joltage: %d\n", joltage);
        #endif
        sum_joltage += joltage;
    }

    printf("%d\n", sum_joltage);

    return 0;
}
