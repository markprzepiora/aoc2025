#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define UNUSED(x) (void)(x)
#define DEFAULT_INPUT "input/day06example.txt"
#define MAX_SEQUENCES (1024*8)
#define MAX_NUMBERS 5

typedef struct Sequence {
    long int numbers[MAX_NUMBERS];
    size_t count;
    char op; // '*' for product, '+' for sum
} Sequence;

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

void read_numbers(FILE *file)
{
    size_t lines_count = 0;
    // read numbers
    while (true) {
        assert(lines_count < MAX_NUMBERS && "number of lines exceeded MAX_NUMBERS");
        lines_count++;
        for (size_t i = 0; i < MAX_SEQUENCES; i++) {
            int next_char = eat_whitespace(file);
            Sequence *sequence = &sequences[i];

            if (next_char == EOF) {
                // If we reach EOF here, we are done reading numbers before any
                // operators were found
                fprintf(stderr, "ERROR: Reached EOF while reading numbers\n");
                exit(1);
            } else if (next_char == '\n') {
                fgetc(file);
                break;
            } else if (next_char == '+' || next_char == '*') {
                // We have reached the operators section
                return;
            } else if (!isdigit(next_char)) {
                // invalid character
                fprintf(stderr, "ERROR: Invalid character '%d' while reading numbers\n", next_char);
                exit(1);
            } else {
                long int number;
                fscanf(file, "%ld", &number);
                sequence->numbers[lines_count - 1] = number;
                sequence->count = lines_count;

                if (sequences_count < i + 1) {
                    sequences_count = i + 1;
                }
            }
        }
    }
}

void read_ops(FILE *file)
{
    for (size_t i = 0; i < MAX_SEQUENCES; i++) {
        Sequence *sequence = &sequences[i];
        int next_char = eat_whitespace(file);
        switch(next_char) {
            case '*':
            case '+': {
                sequence->op = (char) next_char;
                fgetc(file);
            } break;
            case '\n':
            case EOF: {
                return;
            } break;
        }
    }
}

int main(int argc, char **argv)
{
    FILE *file = fopen_r_or_abort(argc >= 2 ? argv[1] : DEFAULT_INPUT);
    long int total = 0;

    read_numbers(file);
    read_ops(file);

    for (size_t i = 0; i < sequences_count; i++) {
        Sequence sequence = sequences[i];
        total += evaluate(sequence);
        #ifdef DEBUG
            log_sequence(sequence);
        #endif
    }

    printf("%ld\n", total);

    return 0;
}
