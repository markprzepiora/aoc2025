#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// #define DEBUG
#include "include/mrp.c"

#define BUFFER_SIZE 256
#define DEFAULT_INPUT "input/day04example.txt"
static char BUFFER[BUFFER_SIZE];

typedef struct Grid {
    bool grid[BUFFER_SIZE][BUFFER_SIZE];
    size_t width;
    size_t height;
} Grid;

static Grid grid = { 0 };

void parse_line(Grid *grid, char *line)
{
    size_t line_len = strlen(line);
    assert(line_len > 0 && "attempted to parse blank line");
    assert(line_len <= BUFFER_SIZE && "attempted to parse line longer than BUFFER_SIZE");
    int row = grid->height++;
    assert(row < BUFFER_SIZE && "attempted to parse more than BUFFER_SIZE lines");
    if (!grid->width) {
        grid->width = line_len;
    }
    assert(grid->width == line_len && "encountered two different line lengths");

    for (size_t n = 0; n < line_len; n++) {
        char symbol = line[n];
        switch(symbol) {
            case '.': {
                grid->grid[row][n] = false;
            } break;
            case '@': {
                grid->grid[row][n] = true;
            } break;
            default: {
                assert(false && "encountered invalid symbol");
            } break;
        }
    }
}

bool occupied(Grid *grid, int row, int col)
{
    if (row < 0 || (size_t) row >= grid->height) {
        return false;
    } else if (col < 0 || (size_t) col >= grid->width) {
        return false;
    } else {
        return grid->grid[row][col];
    }
}

bool reachable(Grid *grid, size_t row, size_t col)
{
    int occupied_count = 0;
    for (int test_row = (int) row - 1; test_row <= (int) row + 1; test_row++) {
        for (int test_col = (int) col - 1; test_col <= (int) col + 1; test_col++) {
            if (test_row == (int) row && test_col == (int) col) continue;
            if (occupied(grid, test_row, test_col)) {
                occupied_count++;
            }
        }
    }
    return occupied_count < 4;
}

int sweep(Grid *grid)
{
    int reachable_count = 0;
    for (size_t row = 0; row < grid->height; row++) {
        for (size_t col = 0; col < grid->width; col++) {
            if (occupied(grid, row, col) && reachable(grid, row, col)) {
                reachable_count++;
                log_debug("x");
            } else {
                log_debug(".");
            }
        }
        log_debug("\n");
    }
    return reachable_count;
}

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);

    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        parse_line(&grid, BUFFER);
    }

    printf("%d\n", sweep(&grid));

    return 0;
}
