#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define SWAP(a, b) {    \
        char *temp = a; \
        a = b;          \
        b = temp;       \
    }
#define DEFAULT_INPUT "input/day07example.txt"
#define BUFFER_SIZE 1024
#define HISTORY_SIZE 256

static char BUFFER_1[1024];
static char BUFFER_2[1024];

typedef struct WorldsTable {
    long int *table;
    size_t cols;
    size_t rows;
    char **history;
    size_t history_len;
} Table;

Table create_table(char **history, size_t history_len)
{
    assert(history_len > 0);
    size_t rows = history_len;
    size_t cols = strlen(history[0]);
    Table table = {
        .table = malloc(sizeof(long int) * rows * cols),
        .rows = rows,
        .cols = cols,
        .history = history,
        .history_len = history_len,
    };
    return table;
}

void table_set(Table *table, size_t row, size_t col, long int value)
{
    table->table[row*table->cols + col] = value;
}

long int table_get(Table *table, size_t row, size_t col)
{
    return table->table[row*table->cols + col];
}

void complete_table(Table *table)
{
    assert(table->rows > 0);
    size_t row = table->rows - 1;
    for (size_t col = 0; col < table->cols; col++) {
        char c = table->history[row][col];
        table_set(table, row, col, c == '|' ? 1 : 0);
    }

    for (int row = (int) table->rows - 2; row >= 0; row--) {
        for (size_t col = 0; col < table->cols; col++) {
            char c = table->history[row][col];
            char below = table->history[row + 1][col];
            if (c == '|' && below == '|') {
                table_set(table, row, col, table_get(table, row + 1, col));
            } else if (c == '|' && below == '^') {
                long int left_value = col > 0 ? table_get(table, row + 1, col - 1) : 0;
                long int right_value = col + 1 < table->cols ? table_get(table, row + 1, col + 1) : 0;
                table_set(table, row, col, left_value + right_value);
            } else {
                table_set(table, row, col, 0);
            }
        }
    }
}

void print_table(Table table)
{
    for (size_t row = 0; row < table.rows; row++) {
        for (size_t col = 0; col < table.cols; col++) {
            fprintf(stderr, "%3ld ", table_get(&table, row, col));
        }
        fprintf(stderr, "\n");
    }
}

long int table_count_worlds(Table table) {
    assert(table.rows > 0);
    long int sum = 0;
    size_t row = 0;
    for (size_t col = 0; col < table.cols; col++) {
        sum += table_get(&table, row, col);
    }
    return sum;
}

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);
    char *buffer = BUFFER_1;
    char *prev_buffer = BUFFER_2;
    char *history[HISTORY_SIZE];
    size_t history_len = 0;

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
            }
        }

        assert(history_len < HISTORY_SIZE && "History buffer overflow");
        history[history_len] = malloc(buffer_len + 1);
        memcpy(history[history_len], buffer, buffer_len + 1);
        history_len++;

        SWAP(buffer, prev_buffer);
    }
    assert(history_len > 0 && "No history recorded");

    for (size_t i = 0; i < history_len; i++) {
        log_debug("%s\n", history[i]);
    }

    Table table = create_table(history, history_len);
    complete_table(&table);

    #ifdef DEBUG
        print_table(table);
    #endif

    printf("%ld\n", table_count_worlds(table));
}
