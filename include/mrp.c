#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define log_debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define log_debug(...) do {} while (0)
#endif

#define debugger __asm__("int3");

FILE *open_input_or_die(const char* filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }
    return file;
}

// Read a line, trimming the trailing newline
char *read_line(FILE *file, char *buffer, size_t buffer_size)
{
    assert(buffer_size > 0 && "buffer size must be positive");
    if (!fgets(buffer, buffer_size, file)) {
        return NULL;
    }
    if (buffer[0] == '\0') {
        return NULL;
    }

    size_t len = strlen(buffer);
    assert((feof(file) || buffer[len-1] == '\n') && "line too long to fit in buffer");

    if (buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    }

    return buffer;
}

bool is_numeric(const char *str)
{
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}
