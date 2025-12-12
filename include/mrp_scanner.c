#ifndef INCLUDE_MRP_SCANNER_C
#define INCLUDE_MRP_SCANNER_C

#include <stdio.h>
#include "./mrp.c"

typedef struct {
    char *buffer;
    char *cursor;
} Scanner;

Scanner scanner_new(char *buffer)
{
    return (Scanner) {
        .buffer = buffer,
        .cursor = buffer,
    };
}

int scanner_pos(Scanner *scanner)
{
    return (int) (scanner->cursor - scanner->buffer);
}

bool eat_char(Scanner *scanner, char c)
{
    if (*scanner->cursor == c) {
        scanner->cursor++;
        return true;
    } else {
        return false;
    }
}

void expect_char_or_die(Scanner *scanner, char c)
{
    if (*scanner->cursor != c) {
        int pos = scanner_pos(scanner);
        fprintf(stderr, "BUFFER: %s\n", scanner->buffer);
        fprintf(stderr, "CURSOR: %*s^\n", pos, "");
        mrp_die("Expected '%c' but got '%c'\n", c, *scanner->cursor);
    }
}

void expect_or_die(Scanner *scanner, bool condition, char *explanation)
{
    if (!condition) {
        int pos = scanner_pos(scanner);
        fprintf(stderr, "BUFFER: %s\n", scanner->buffer);
        fprintf(stderr, "CURSOR: %*s^\n", pos, "");
        mrp_die("%s\n", explanation);
    }
}

void eat_char_or_die(Scanner *scanner, char c)
{
    expect_char_or_die(scanner, c);
    eat_char(scanner, c);
}

size_t scan_until_ch(Scanner *scanner, char *buffer, size_t buffer_len, char sep)
{
    if (!*scanner->cursor || *scanner->cursor == sep) return 0;
    size_t i = 0;
    for (i = 0; i < buffer_len - 1; i++) {
        char ch = *(scanner->cursor+i);
        if (!ch || ch == sep) {
            *(buffer+i) = '\0';
            break;
        } else {
            *(buffer+i) = *(scanner->cursor+i);
        }
    }
    scanner->cursor += i;
    return i;
}

#endif // INCLUDE_MRP_SCANNER_C
