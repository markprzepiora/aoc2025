#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define UNUSED(x) (void)(x)
#define CTOI(c) ((c) - '0')
#define DEFAULT_INPUT "input/day06example.txt"
#define MAX_SEQUENCES (1024*8)
#define MAX_NUMBERS 5

typedef struct Sequence {
    long int numbers[MAX_NUMBERS];
    size_t count;
    char op; // '*' for product, '+' for sum
} Sequence;

typedef struct Table {
    char *contents;
    size_t cols_len;
    size_t rows_len;
} Table;

static Sequence sequences[MAX_SEQUENCES] = { 0 };
static size_t sequences_count = 0;

long int evaluate_sum(Sequence seq)
{
    long int result = 0;
    for (size_t i = 0; i < seq.count; i++) {
        result += seq.numbers[i];
    }
    return result;
}

long int evaluate_product(Sequence seq)
{
    long int result = 1;
    for (size_t i = 0; i < seq.count; i++) {
        result *= seq.numbers[i];
    }
    return result;
}

long int evaluate(Sequence seq)
{
    switch(seq.op) {
        case '+': {
            return evaluate_sum(seq);
        } break;
        case '*': {
            return evaluate_product(seq);
        } break;
        default: {
            fprintf(stderr, "ERROR: invalid op: %c", seq.op);
            exit(1);
        }
    }
}

void log_sequence(Sequence sequence)
{
    for (size_t i = 0; i < sequence.count; i++) {
        fprintf(stderr, "%ld", sequence.numbers[i]);
        if (i < sequences_count - 2) {
            fprintf(stderr, " %c ", sequence.op);
        }
    }
    fprintf(stderr, " = %ld\n", evaluate(sequence));
}

char table_get(Table table, size_t col, size_t row)
{
    return table.contents[row*(table.cols_len+1) + col];
}

bool col_is_sep(Table table, size_t col)
{
    for (size_t row = 0; row < table.rows_len; row++) {
        if (table_get(table, col, row) != ' ') {
            return false;
        }
    }
    return true;
}

Table read_table(FILE *file)
{
    Table table = {0};
    table.contents = read_file(file);

    // compute column count
    char *line_end = strchr(table.contents, '\n');
    assert(line_end && "First line does not contain a newline");
    table.cols_len = line_end - table.contents;
    table.rows_len = 1;

    char *line_end_prev = line_end + 1;
    while ((line_end = strchr(line_end_prev, '\n'))) {
        assert((size_t) (line_end - line_end_prev) == table.cols_len && "encountered multiple line lengths");
        table.rows_len++;
        line_end_prev = line_end + 1;
    }

    return table;
}

void read_numbers(Table table)
{
    size_t sequence_idx = 0;
    size_t number_idx = 0;

    for (size_t row = 0; row < table.rows_len - 1; row++) {
        for (size_t col = 0; col < table.cols_len; col++) {
            if (col_is_sep(table, col)) {
                log_debug("separator at col %zu\n", col);
                sequence_idx++;
                number_idx = 0;
                continue;
            }
            Sequence *seq = &sequences[sequence_idx];
            sequences_count = MAX(sequence_idx + 1, sequences_count);
            char c = table_get(table, col, row);
            if (isdigit(c)) {
                log_debug("(seq %zu, num %zu) Adding digit %c to end of %ld\n",
                          sequence_idx, number_idx, c, seq->numbers[number_idx]);
                seq->numbers[number_idx] = seq->numbers[number_idx]*10 + CTOI(c);
            }
            seq->count = MAX(number_idx + 1, seq->count);
            number_idx++;
        }
        sequence_idx = 0;
        number_idx = 0;
    }
}

void read_ops(Table table)
{
    assert(table.rows_len > 0);
    size_t row = table.rows_len - 1;
    size_t sequence_idx = 0;
    for (size_t col = 0; col < table.cols_len; col++) {
        if (col == 0 || col_is_sep(table, col - 1)) {
            char op = table_get(table, col, row);
            assert(op == '*' || op == '+');
            Sequence *seq = &sequences[sequence_idx];
            assert(sequence_idx < MAX_SEQUENCES);
            seq->op = op;
            sequence_idx++;
        }
    }
}

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);
    long int total = 0;

    Table table = read_table(file);
    log_debug("table.cols_len = %zu\n", table.cols_len);
    log_debug("table.rows_len = %zu\n", table.rows_len);
    read_numbers(table);
    read_ops(table);

    for (size_t i = 0; i < sequences_count; i++) {
        Sequence sequence = sequences[i];
        #ifdef DEBUG
            log_sequence(sequence);
        #endif
        total += evaluate(sequence);
    }

    printf("%ld\n", total);

    return 0;
}
