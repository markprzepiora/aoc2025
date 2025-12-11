#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"
#include "include/matrix.c"

#define BUFFER_SIZE 1024
#define MAX_MACHINES 256
#define MAX_BUTTONS 20
static char BUFFER[1024];

typedef struct {
    int target;
    int target_char_len;
    int buttons[MAX_BUTTONS];
    int buttons_len;
    int joltage_target[MAX_BUTTONS];
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

        eat_char_or_die(&scanner, '{');

        int joltage_target_len = 0;
        while (true) {
            expect_or_die(&scanner, isdigit(*scanner.cursor), "Expected number in joltage target definition");
            int joltage_target = 0;
            while (isdigit(*scanner.cursor)) {
                joltage_target = joltage_target * 10 + (*scanner.cursor - '0');
                scanner.cursor++;
            }
            machine->joltage_target[joltage_target_len++] = joltage_target;
            if (*scanner.cursor == '}') break;
            eat_char_or_die(&scanner, ',');
        }

        assert(joltage_target_len == machine->target_char_len);

        eat_char_or_die(&scanner, '}');

        machines_len++;
    }
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
    log_debug("  Joltage Targets: ");
    for (int i = 0; i < machine.target_char_len; i++) {
        log_debug("%d ", machine.joltage_target[i]);
    }
    log_debug("\n");
}

bool verify_solution(Machine machine, int *solution)
{
    int computed_joltage[MAX_BUTTONS] = {0};
    for (int button_idx = 0; button_idx < machine.buttons_len; button_idx++) {
        int presses = solution[button_idx];
        int button_mask = machine.buttons[button_idx];
        for (int bit_idx = 0; bit_idx < machine.target_char_len; bit_idx++) {
            int bit = (button_mask >> (machine.target_char_len - 1 - bit_idx)) & 1;
            if (bit) {
                computed_joltage[bit_idx] += presses;
            }
        }
    }
    for (int i = 0; i < machine.target_char_len; i++) {
        if (computed_joltage[i] != machine.joltage_target[i]) {
            log_debug("Verification failed at index %d: expected %d, got %d\n", i, machine.joltage_target[i], computed_joltage[i]);
            return false;
        }
    }
    return true;
}

Matrix build_solution_matrix(Machine machine)
{
    Matrix m = matrix_new(machine.target_char_len, machine.buttons_len + 1);

    // each button fills out one column of the matrix
    for (int col = 0; col < machine.buttons_len; col++) {
        for (int row = 0; row < machine.target_char_len; row++) {
            int bit = (machine.buttons[col] >> (machine.target_char_len - 1 - row)) & 1;
            matrix_set(&m, row, col, (double) bit);
        }
    }

    // the final column is filled with the joltage targets
    for (int row = 0; row < machine.target_char_len; row++) {
        matrix_set(&m, row, machine.buttons_len, (double) machine.joltage_target[row]);
    }

    return m;
}

int machine_max_joltage_target(Machine machine) {
    int max = 0;
    for (int i = 0; i < machine.target_char_len; i++) {
        if (machine.joltage_target[i] > max) {
            max = machine.joltage_target[i];
        }
    }
    return max;
}

// You should think of this as a base-(max_value + 1) counter with len digits
typedef struct {
    int vals[MAX_BUTTONS];
    size_t len;
    int max_value;
} Assignments;

bool assignments_inc(Assignments *assignments)
{
    for (size_t i = 0; i < assignments->len; i++) {
        if (assignments->vals[i] < assignments->max_value) {
            assignments->vals[i]++;
            return true;
        } else {
            assignments->vals[i] = 0;
        }
    }
    return false;
}

void copy_solution(double *solution_src, int *solution_target)
{
    if (!solution_target) return;
    for (int i = 0; i < MAX_BUTTONS; i++) {
        solution_target[i] = (int) round(solution_src[i]);
    }
}

void debug_log_solution(int *solution, size_t len)
{
    UNUSED(solution);
    log_debug("Solution:\n");
    for (size_t i = 0; i < len; i++) {
        log_debug("  Button %zu: %d presses\n", i, solution[i]);
    }
}

int solve(Machine machine, int* best_solution) {
    Matrix matrix = build_solution_matrix(machine);
    #ifdef DEBUG
        matrix_print(matrix);
    #endif
    gaussian_elimination(&matrix);
    log_debug("----\n");
    #ifdef DEBUG
        matrix_print(matrix);
    #endif
    // assert(matrix.independent_cols_len == 1);
    int max_joltage_target = machine_max_joltage_target(machine);
    int min_sum = -1;

    Assignments assignments = {
        .len = (size_t) matrix.independent_cols_len,
        .max_value = max_joltage_target,
    };

    Matrix best_solution_matrix = matrix_extend_rows(matrix, (int) assignments.len);

    do {
        double solution[MAX_BUTTONS] = {0};
        Matrix copy = matrix_extend_rows(matrix, (int) assignments.len);
        for (size_t i = 0; i < assignments.len; i++) {
            int col = matrix.independent_cols[i];
            int value = assignments.vals[i];
            matrix_set(&copy, matrix.rows + (int) i, col, 1.0f);
            matrix_set(&copy, matrix.rows + (int) i, copy.cols - 1, (double) value);
        }
        gaussian_elimination(&copy);
        back_substitution(copy, solution);
        int sum = 0;
        bool invalid_solution = false;
        for (int i = 0; i < copy.rows; i++) {
            double solution_val = solution[i];
            // log_debug("  Variable %d: %f\n", i, solution_val);
            if (isnan(solution_val)) {
                continue;
            }
            if (solution_val < -0.0001f || fabs(round(solution_val) - solution_val) > 0.0001f) {
            // if (solution_val < 0.0f) {
                invalid_solution = true;
                break;
            }
            sum += (int) round(solution_val);
        }
        if (invalid_solution) {
            matrix_free(&copy);
            continue;
        }
        if (min_sum == -1 || (sum > 0 && sum < min_sum)) {
            copy_solution(solution, best_solution);
            matrix_copy_into(copy, best_solution_matrix);
            min_sum = sum;
        }
        matrix_free(&copy);
    } while(assignments_inc(&assignments));

    log_debug("Best solution matrix:\n");
    #ifdef DEBUG
        matrix_print(best_solution_matrix);
    #endif
    debug_log_solution(best_solution, (size_t) machine.buttons_len);
    assert(verify_solution(machine, best_solution));

    return min_sum;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    parse_input(input_file);

    int sum = 0;
    for (size_t i = 0; i < machines_len; i++) {
        log_debug("Machine %zu\n", i);
        Machine machine = machines[i];
        int best_solution[MAX_BUTTONS] = {0};
        int min_presses = solve(machine, best_solution);
        log_debug("Machine %zu: Minimum button presses = %d\n", i, min_presses);
        assert(min_presses > 0);
        sum += min_presses;
    }

    printf("%d\n", sum);

    return 0;
}
