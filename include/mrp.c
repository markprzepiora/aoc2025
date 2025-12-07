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
#define debug_unless(cond) if (!(cond)) debugger
#define ARRAYLEN(arr)  (sizeof(arr) / sizeof((arr)[0]))

FILE *fopen_r_or_abort(const char* filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }
    return file;
}

char *read_file(FILE *file)
{
    unsigned char buffer[128];
    size_t ret;
    size_t contents_size = 0;
    size_t contents_capacity = 1;
    char *contents = malloc(1);
    contents[0] = '\0';

    while ((ret = fread(buffer, sizeof(*buffer), ARRAYLEN(buffer), file))) {
        if (contents_size + ret + 1 > contents_capacity) {
            contents_capacity = (contents_size + ret + 1) * 2;
            contents = realloc(contents, contents_capacity);
        }
        memcpy(contents + contents_size, buffer, ret);
        contents_size += ret;
        contents[contents_size] = '\0';
    }

    return contents;
}

// returns the first non-whitespace character without consuming it
char eat_whitespace(FILE *file) {
    int c;
    do {
        c = fgetc(file);
    } while (isspace(c) && c != '\n');
    if (c != EOF) {
        ungetc(c, file);
    }
    return c;
}

char peek(FILE *file) {
    int c = fgetc(file);
    ungetc(c, file);
    return c;
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
