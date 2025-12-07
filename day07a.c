#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define UNUSED(x) (void)(x)
#define CTOI(c) ((c) - '0')
#define SWAP(a, b) {    \
        char *temp = a; \
        a = b;          \
        b = temp;       \
    }

#define DEFAULT_INPUT "input/day07example.txt"
#define BUFFER_SIZE 1024

static char BUFFER_1[1024];
static char BUFFER_2[1024];

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);
    char *buffer = BUFFER_1;
    char *prev_buffer = BUFFER_2;

    int split_count = 0;

    read_line(file, prev_buffer, BUFFER_SIZE);
    while (read_line(file, buffer, BUFFER_SIZE)) {
        size_t buffer_len = strlen(buffer);
        size_t prev_buffer_len = strlen(prev_buffer);

        assert(buffer_len == prev_buffer_len && "Input lines must be of equal length");

        for (size_t i = 0; i < buffer_len; i++) {
            char prev_char_above = prev_buffer[i];
            char current_char = buffer[i];

            if (prev_char_above == 'S' && current_char == '.') {
                buffer[i] = '|';
            } else if (prev_char_above == '|' && current_char == '.') {
                buffer[i] = '|';
            } else if (prev_char_above == '|' && current_char == '^') {
                if (i > 0) {
                    buffer[i-1] = '|';
                }
                if (i + 1 < buffer_len) {
                    buffer[i+1] = '|';
                }
                split_count++;
            }
        }

        log_debug("processing: '%s' after '%s'\n", buffer, prev_buffer);

        SWAP(buffer, prev_buffer);
    }

    printf("%d\n", split_count);
}
