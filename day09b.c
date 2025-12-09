#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

// #define DEBUG
#include "include/mrp.c"

#define MAX_POINTS 1024

typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int orig_x;
    unsigned int orig_y;
} Point;

typedef struct {
    Point *points;
    size_t size;
} PointList;

typedef struct {
    char *grid;
    size_t width;
    size_t height;
} Grid;

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

PointList parse_input(char *input_file)
{
    FILE *file = fopen_r_or_abort(input_file);
    unsigned int x, y;
    while (fscanf(file, "%u,%u\n", &x, &y) == 2) {
        assert(points_count < MAX_POINTS);
        points[points_count] = (Point){
            .x = x,
            .y = y,
            .orig_x = x,
            .orig_y = y,
        };
        points_count++;
    }
    return (PointList) {
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

static int cmp_box_area_reverse(const Box *box1, const Box *box2)
{
    if (box1->area < box2->area) {
        return 1;
    } else if (box1->area > box2->area) {
        return -1;
    } else {
        return 0;
    }
}

Boxes build_boxes(PointList point_list)
{
    Boxes boxes = {0};
    boxes.size = (point_list.size * (point_list.size - 1)) / 2;
    boxes.boxes = calloc((size_t) boxes.size, sizeof(Box));

    size_t boxes_idx = 0;
    for (size_t i = 0; i < point_list.size; i++) {
        for (size_t j = 0; j < i; j++) {
            assert(boxes_idx < boxes.size);
            boxes.boxes[boxes_idx] = box_new(&point_list.points[i], &point_list.points[j]);
            boxes_idx++;
        }
    }
    qsort(boxes.boxes, boxes.size, sizeof(Box), (int (*)(const void *, const void *)) cmp_box_area_reverse);
    return boxes;
}

void debug_log_point(Point *point)
{
    UNUSED(point);
    log_debug("Point(%u, %u, orig = %u, %u)\n", point->x, point->y, point->orig_x, point->orig_y);
}

void debug_log_point_list(PointList point_list)
{
    for (size_t i = 0; i < point_list.size; i++) {
        debug_log_point(&point_list.points[i]);
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

Grid grid_create(PointList point_list)
{
    Grid grid = {0};

    size_t max_x = 0;
    size_t max_y = 0;
    for (size_t i = 0; i < point_list.size; i++) {
        if (point_list.points[i].x > max_x) {
            max_x = point_list.points[i].x;
        }
        if (point_list.points[i].y > max_y) {
            max_y = point_list.points[i].y;
        }
    }

    grid.width = max_x + 1;
    grid.height = max_y + 1;
    grid.grid = calloc((size_t) grid.height * grid.width, sizeof(char));
    memset(grid.grid, '.', grid.height * grid.width);

    return grid;
}

char grid_get(Grid grid, size_t x, size_t y)
{
    assert(x < grid.width);
    assert(y < grid.height);
    return grid.grid[y * grid.width + x];
}

void grid_set(Grid grid, size_t x, size_t y, char value)
{
    assert(x < grid.width);
    assert(y < grid.height);
    // assert(grid_get(grid, x, y) != 'X');
    // assert(grid_get(grid, x, y) != '#');
    grid.grid[y * grid.width + x] = value;
}

void grid_connect_points(Grid grid, Point p1, Point p2)
{
    assert(p1.x == p2.x || p1.y == p2.y);
    grid_set(grid, p1.x, p1.y, '#');
    grid_set(grid, p2.x, p2.y, '#');
    if (p1.x == p2.x) {
        size_t start_y = MIN(p1.y, p2.y);
        size_t end_y = MAX(p1.y, p2.y);
        for (size_t y = start_y + 1; y < end_y; y++) {
            grid_set(grid, p1.x, y, 'X');
        }
    } else if (p1.y == p2.y) {
        size_t start_x = MIN(p1.x, p2.x);
        size_t end_x = MAX(p1.x, p2.x);
        for (size_t x = start_x + 1; x < end_x; x++) {
            grid_set(grid, x, p1.y, 'X');
        }
    }
}

void grid_connect(Grid grid, PointList point_list)
{
    assert(point_list.size >= 2);
    for (size_t i = 1; i < point_list.size; i++) {
        Point prev_point = point_list.points[i-1];
        Point point = point_list.points[i];
        grid_connect_points(grid, prev_point, point);
    }
    grid_connect_points(grid, point_list.points[point_list.size - 1], point_list.points[0]);
}

void grid_fill(Grid grid, size_t x, size_t y, char fill_char)
{
    if (x >= grid.width || y >= grid.height) {
        return;
    }
    char current_char = grid_get(grid, x, y);
    if (current_char != '.') {
        return;
    }
    grid_set(grid, x, y, fill_char);
    grid_fill(grid, x + 1, y, fill_char);
    if (x > 0) {
        grid_fill(grid, x - 1, y, fill_char);
    }
    grid_fill(grid, x, y + 1, fill_char);
    if (y > 0) {
        grid_fill(grid, x, y - 1, fill_char);
    }
}

bool rectangle_inside_grid(Grid grid, Point p1, Point p2)
{
    size_t min_x = MIN(p1.x, p2.x);
    size_t max_x = MAX(p1.x, p2.x);
    size_t min_y = MIN(p1.y, p2.y);
    size_t max_y = MAX(p1.y, p2.y);

    for (size_t y = min_y; y <= max_y; y++) {
        for (size_t x = min_x; x <= max_x; x++) {
            char c = grid_get(grid, x, y);
            if (c == ' ') {
                return false;
            }
        }
    }
    return true;
}

Box *find_biggest_box(Boxes boxes, Grid grid)
{
    for (size_t box_idx = 0; box_idx < boxes.size; box_idx++) {
        Box *box = &boxes.boxes[box_idx];
        log_debug("Testing box:\n");
        debug_log_box(*box);
        if (rectangle_inside_grid(grid, *box->p1, *box->p2)) {
            return box;
        }
    }
    return NULL;
}

void debug_log_grid(Grid grid) {
    log_debug("Width: %zu, Height: %zu\n", grid.width, grid.height);
    for (size_t y = 0; y < grid.height; y++) {
        log_debug("%.*s\n", (int)grid.width, &grid.grid[y * grid.width]);
    }
}

static int cmp_point_x(const Point **p1, const Point **p2)
{
    if ((*p1)->x < (*p2)->x) {
        return -1;
    } else if ((*p1)->x > (*p2)->x) {
        return 1;
    } else {
        return 0;
    }
}

static int cmp_point_y(const Point **p1, const Point **p2)
{
    if ((*p1)->y < (*p2)->y) {
        return -1;
    } else if ((*p1)->y > (*p2)->y) {
        return 1;
    } else {
        return 0;
    }
}

Point **sort_points(PointList point_list, int (*cmp)(const Point **, const Point **))
{
    Point **points = calloc(point_list.size, sizeof(Point*));
    for (size_t i = 0; i < point_list.size; i++) {
        points[i] = &point_list.points[i];
    }
    qsort(points, point_list.size, sizeof(Point*), (int (*)(const void *, const void *)) cmp);
    return points;
}

void renumber_xs(PointList point_list)
{
    Point **points_sorted_by_x = sort_points(point_list, cmp_point_x);
    // Renumber x coordinates
    unsigned int prev_x = points_sorted_by_x[0]->x;
    unsigned int new_x = 1;
    for (size_t i = 0; i < point_list.size; i++) {
        unsigned int x = points_sorted_by_x[i]->x;
        if (x != prev_x) {
            new_x += 2;
        }
        points_sorted_by_x[i]->x = new_x;
        prev_x = x;
    }
    free(points_sorted_by_x);
}

void renumber_ys(PointList point_list)
{
    Point **points_sorted_by_y = sort_points(point_list, cmp_point_y);
    // Renumber y coordinates
    unsigned int prev_y = points_sorted_by_y[0]->y;
    unsigned int new_y = 1;
    for (size_t i = 0; i < point_list.size; i++) {
        unsigned int y = points_sorted_by_y[i]->y;
        if (y != prev_y) {
            new_y += 2;
        }
        points_sorted_by_y[i]->y = new_y;
        prev_y = y;
    }
    free(points_sorted_by_y);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    char *input_file = argv[1];
    PointList point_list = parse_input(input_file);
    Boxes boxes = build_boxes(point_list);
    renumber_xs(point_list);
    renumber_ys(point_list);
    debug_log_point_list(point_list);

    Grid grid = grid_create(point_list);
    grid_connect(grid, point_list);
    grid_fill(grid, 0, 0, ' ');
    grid_fill(grid, grid.width - 1, 0, ' ');
    grid_fill(grid, 0, grid.height - 1, ' ');
    grid_fill(grid, grid.width - 1, grid.height - 1, ' ');
    debug_log_grid(grid);

    Box biggest_box = *find_biggest_box(boxes, grid);
    printf("%ld\n", biggest_box.area);

    return 0;
}
