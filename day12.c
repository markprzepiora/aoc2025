// #define DEBUG
#include "include/mrp.c"
#include "include/mrp_scanner.c"
#include <sys/param.h>

#define BUFFER_SIZE 1024
static char BUFFER[BUFFER_SIZE];

#define SHAPES_COUNT 6
#define MAX_PROBLEMS_COUNT 2048

typedef struct {
    int size;
} Shape;

typedef struct {
    int width;
    int height;
    int shape_counts[SHAPES_COUNT];
} Problem;

static Shape SHAPES[SHAPES_COUNT];
static Problem PROBLEMS[MAX_PROBLEMS_COUNT];
static size_t PROBLEMS_COUNT = 0;

void parse_input(char *path)
{
    FILE *file = fopen_r_or_abort(path);

    for (int shape_idx = 0; shape_idx < SHAPES_COUNT; shape_idx++) {
        int shape_size = 0;
        read_line(file, BUFFER, BUFFER_SIZE);
        mrp_assert(*BUFFER, "Unexpected end of file while reading shape header");
        mrp_assert(isdigit(BUFFER[0]), "Shape header must start with a digit");
        mrp_assert(BUFFER[1] == ':', "Shape header must be followed by ':'");
        for (int line_idx = 0; line_idx < 3; line_idx++) {
            read_line(file, BUFFER, BUFFER_SIZE);
            mrp_assert(*BUFFER, "Unexpected end of file while reading shape body");
            mrp_assert(strlen(BUFFER) == 3, "Shape body line must be 3 characters long");
            shape_size +=
                (BUFFER[0] == '#' ? 1 : 0) +
                (BUFFER[1] == '#' ? 1 : 0) +
                (BUFFER[2] == '#' ? 1 : 0);
        }
        read_line(file, BUFFER, BUFFER_SIZE);
        mrp_assert(strlen(BUFFER) == 0, "Expected empty line after shape body");
        SHAPES[shape_idx].size = shape_size;
    }

    while (true) {
        mrp_assert(PROBLEMS_COUNT < MAX_PROBLEMS_COUNT, "Exceeded maximum problems count");
        Scanner scanner = read_line_as_scanner(file, BUFFER, BUFFER_SIZE);
        if (!scanner.buffer) {
            break;
        }
        int width = eat_positive_int_or_die(&scanner);
        eat_char_or_die(&scanner, 'x');
        int height = eat_positive_int_or_die(&scanner);
        eat_char_or_die(&scanner, ':');
        for (int shape_idx = 0; shape_idx < SHAPES_COUNT; shape_idx++) {
            eat_char_or_die(&scanner, ' ');
            int count = eat_positive_int_or_die(&scanner);
            PROBLEMS[PROBLEMS_COUNT].shape_counts[shape_idx] = count;
        }
        PROBLEMS[PROBLEMS_COUNT].width = width;
        PROBLEMS[PROBLEMS_COUNT].height = height;
        PROBLEMS_COUNT++;
    }
}

int problem_grid_area(Problem problem)
{
    return problem.width * problem.height;
}

int problem_total_shape_area(Problem problem)
{
    int total_area = 0;
    for (int shape_idx = 0; shape_idx < SHAPES_COUNT; shape_idx++) {
        total_area += SHAPES[shape_idx].size * problem.shape_counts[shape_idx];
    }
    return total_area;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    parse_input(input_file);

    int fit_sum = 0;
    for (size_t problem_idx = 0; problem_idx < PROBLEMS_COUNT; problem_idx++) {
        Problem problem = PROBLEMS[problem_idx];
        int grid_area = problem_grid_area(problem);
        int shapes_area = problem_total_shape_area(problem);
        if (shapes_area <= grid_area) {
            fit_sum += 1;
        }
    }

    printf("%d\n", fit_sum);

    return 0;
}
