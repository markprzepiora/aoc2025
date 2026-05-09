#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define BUFFER_SIZE 1024
#define MAX_MACHINES 256
static char BUFFER[1024];

typedef struct {
    int target;
    int target_char_len;
    int buttons[20];
    int buttons_len;
    int idx; // only for debugging
} Machine;

typedef struct {
    char *buffer;
    char *cursor;
} Scanner;

static Machine machines[MAX_MACHINES] = {0};
static size_t machines_len = 0;

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

// Sets the nth bit (from the left, 0-indexed) of mask to 1
int set_nth_1(int mask, int mask_len, int digit)
{
    int shift_count = mask_len - digit - 1;
    return mask | (1 << shift_count);
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

void parse_input(char *input_file)
{
    FILE *file = fopen_r_or_abort(input_file);
    while (read_line(file, BUFFER, BUFFER_SIZE)) {
        Scanner scanner = scanner_new(BUFFER);
        Machine *machine = &machines[machines_len];
        eat_char_or_die(&scanner, '[');
        while (*scanner.cursor == '#' || *scanner.cursor == '.') {
            machine->target_char_len++;
            machine->target <<= 1;
            machine->idx = (int) machines_len; // only for debugging
            if (*scanner.cursor == '#') machine->target |= 1;
            scanner.cursor++;
        }
        eat_char_or_die(&scanner, ']');
        eat_char_or_die(&scanner, ' ');

        while (true) {
            if (*scanner.cursor == '{') break;
            eat_char_or_die(&scanner, '(');
            int button_mask = 0;
            while (true) {
                expect_or_die(&scanner, isdigit(*scanner.cursor), "Expected number in button definition");
                button_mask = set_nth_1(button_mask, machine->target_char_len, *scanner.cursor - '0');
                scanner.cursor++;
                if (*scanner.cursor == ')') break;
                eat_char_or_die(&scanner, ',');
            }
            machine->buttons[machine->buttons_len++] = button_mask;
            eat_char_or_die(&scanner, ')');
            eat_char_or_die(&scanner, ' ');
        }

        machines_len++;
    }
}

// Counts the number of bits set to 1 in n
int num_bits_set(int n)
{
    int result = 0;
    while (n) {
        result += n & 1;
        n >>= 1;
    }
    return result;
}

int press_buttons(Machine machine, int buttons_mask)
{
    int result = 0;
    for (int i = machine.buttons_len - 1; i >= 0; i--) {
        if (buttons_mask & 1) {
            result ^= machine.buttons[i];
        }
        buttons_mask >>= 1;
    }
    return result;
}

int solve(Machine machine)
{
    // if we have 5 buttons then we need to go through
    // 00000 to 11111 to try them all
    int max_buttons_mask = ~((~0 >> machine.buttons_len) << machine.buttons_len);

    int min_presses = -1;

    for (int mask = 0; mask <= max_buttons_mask; mask++) {
        int result = press_buttons(machine, mask);
        if (result == machine.target) {
            int presses = num_bits_set(mask);
            if (min_presses == -1 || presses < min_presses) {
                min_presses = presses;
            }
        }
    }
    return min_presses;
}

void debug_log_machine(Machine machine)
{
    log_debug("Machine %d:\n", machine.idx);
    log_debug("  Target: ");
    for (int i = machine.target_char_len - 1; i >= 0; i--) {
        int bit = (machine.target >> i) & 1;
        UNUSED(bit);
        log_debug("%c", bit ? '#' : '.');
    }
    log_debug("\n");
    log_debug("  Buttons:\n");
    for (int i = 0; i < machine.buttons_len; i++) {
        log_debug("    Button %d: ", i);
        for (int j = machine.target_char_len - 1; j >= 0; j--) {
            int bit = (machine.buttons[i] >> j) & 1;
            UNUSED(bit);
            log_debug("%c", bit ? '#' : '.');
        }
        log_debug("\n");
    }
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    parse_input(input_file);

    int result = 0;
    for (size_t i = 0; i < machines_len; i++) {
        debug_log_machine(machines[i]);
        int answer = solve(machines[i]);
        result += answer;
    }
    printf("%d\n", result);

    return 0;
}
