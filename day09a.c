#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// #define DEBUG
#include "include/mrp.c"

#define MAX_POINTS 1024

typedef struct {
    unsigned int x;
    unsigned int y;
} Point;

typedef struct {
    Point *points;
    size_t size;
} PointSet;

typedef struct {
    Point *p1;
    Point *p2;
    long int area;
} Box;

typedef struct {
    Box *boxes;
    size_t size;
} Boxes;

static Point points[MAX_POINTS];
static size_t points_count = 0;

PointSet parse_input(char *input_file)
{
    FILE *file = fopen_r_or_abort(input_file);
    unsigned int x, y;
    while (fscanf(file, "%u,%u\n", &x, &y) == 2) {
        assert(points_count < MAX_POINTS);
        points[points_count] = (Point){
            .x = x,
            .y = y,
        };
        points_count++;
    }
    return (PointSet) {
        .points = points,
        .size = points_count,
    };
}

Box box_new(Point *p1, Point *p2)
{
    long int area =
        (labs((long int)p1->x - (long int)p2->x) + 1) *
        (labs((long int)p1->y - (long int)p2->y) + 1);
    return (Box){
        .p1 = p1,
        .p2 = p2,
        .area = area,
    };
}

Boxes build_boxes(PointSet point_set)
{
    Boxes boxes = {0};
    boxes.size = (point_set.size * (point_set.size - 1)) / 2;
    boxes.boxes = calloc((size_t) boxes.size, sizeof(Box));

    size_t boxes_idx = 0;
    for (size_t i = 0; i < point_set.size; i++) {
        for (size_t j = 0; j < i; j++) {
            assert(boxes_idx < boxes.size);
            boxes.boxes[boxes_idx] = box_new(&point_set.points[i], &point_set.points[j]);
            boxes_idx++;
        }
    }
    return boxes;
}

void debug_log_point(Point *point)
{
    UNUSED(point);
    log_debug("Point(%u, %u)\n", point->x, point->y);
}

void debug_log_point_set(PointSet point_set)
{
    for (size_t i = 0; i < point_set.size; i++) {
        debug_log_point(&point_set.points[i]);
    }
}

void debug_log_box(Box box)
{
    log_debug("Box: ");
    debug_log_point(box.p1);
    log_debug(" - ");
    debug_log_point(box.p2);
    log_debug(" Area: %ld\n", box.area);
}

Box find_biggest_box(Boxes boxes)
{
    assert(boxes.size > 0);
    Box biggest_box = boxes.boxes[0];
    for (size_t i = 0; i < boxes.size; i++) {
        if (boxes.boxes[i].area > biggest_box.area) {
            biggest_box = boxes.boxes[i];
        }
    }
    return biggest_box;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    PointSet point_set = parse_input(input_file);

    debug_log_point_set(point_set);

    Boxes boxes = build_boxes(point_set);

    debug_log_box(boxes.boxes[0]);

    Box biggest_box = find_biggest_box(boxes);

    printf("%ld\n", biggest_box.area);

    return 0;
}
