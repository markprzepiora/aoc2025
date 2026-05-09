#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// #define DEBUG
#include "include/mrp.c"

#define MAX_BOXES 1024

typedef struct {
    float x;
    float y;
    float z;
    int circuit;
} Box;

typedef struct {
    int box1_idx;
    int box2_idx;
    float distance;
} BoxPair;

typedef struct {
    BoxPair *pairs;
    int count;
} BoxPairs;

typedef struct {
    int id;
    int boxes_count;
} Circuit;

static Box boxes[MAX_BOXES];
static int boxes_count = 0;
static Circuit circuits[MAX_BOXES] = { 0 };

float distance_squared(Box a, Box b)
{
    return (a.x - b.x) * (a.x - b.x) +
           (a.y - b.y) * (a.y - b.y) +
           (a.z - b.z) * (a.z - b.z);
}

void print_box(Box v)
{
    fprintf(stderr, "(%f, %f, %f)\n", v.x, v.y, v.z);
}

void parse_input(char *input_file)
{
    FILE *file = fopen_r_or_abort(input_file);
    float x, y, z;
    while (fscanf(file, "%f,%f,%f\n", &x, &y, &z) == 3) {
        assert(boxes_count < MAX_BOXES);
        boxes[boxes_count] = (Box){
            .x = x,
            .y = y,
            .z = z,
            .circuit = boxes_count,
        };
        circuits[boxes_count] = (Circuit) {
            .id = boxes_count,
            .boxes_count = 1,
        };
        boxes_count++;
    }
}

static int cmpboxpair(const BoxPair *p1, const BoxPair *p2)
{
    if (p1->distance < p2->distance) {
        return -1;
    } else if (p1->distance > p2->distance) {
        return 1;
    } else {
        return 0;
    }
}

void print_pair(BoxPair p)
{
    fprintf(stderr, "Distance: %f\n", p.distance);
    fprintf(stderr, "Box 1: ");
    print_box(boxes[p.box1_idx]);
    fprintf(stderr, "Box 2: ");
    print_box(boxes[p.box2_idx]);
}

BoxPairs build_box_pairs()
{
    BoxPairs pairs = {0};
    pairs.count = (boxes_count * (boxes_count - 1)) / 2;
    pairs.pairs = calloc((size_t) pairs.count, sizeof(BoxPair));

    int pairs_idx = 0;
    for (int i = 0; i < boxes_count; i++) {
        for (int j = 0; j < i; j++) {
            assert(pairs_idx < pairs.count);
            pairs.pairs[pairs_idx] = ((BoxPair){
                .box1_idx = i,
                .box2_idx = j,
                .distance = distance_squared(boxes[i], boxes[j]),
            });
            pairs_idx++;
        }
    }
    qsort(pairs.pairs, (size_t) pairs.count, sizeof(BoxPair), (int (*)(const void *, const void *)) cmpboxpair);
    return pairs;
}

void connect_boxes(Box *b1, Box *b2)
{
    int b1_circuit = b1->circuit;
    int b2_circuit = b2->circuit;

    if (b1_circuit == b2_circuit) {
        return;
    }

    for (int i = 0; i < boxes_count; i++) {
        if (boxes[i].circuit == b2_circuit) {
            boxes[i].circuit = b1_circuit;
        }
    }
    circuits[b1_circuit].boxes_count += circuits[b2_circuit].boxes_count;
    circuits[b2_circuit].boxes_count = 0;
}

bool all_connected()
{
    int circuit = boxes[0].circuit;
    for (int i = 1; i < boxes_count; i++) {
        if (boxes[i].circuit != circuit) {
            return false;
        }
    }
    return true;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];

    parse_input(input_file);
    for (int i = 0; i < boxes_count; i++) {
        #ifdef DEBUG
            print_box(boxes[i]);
        #endif
    }

    BoxPairs pairs = build_box_pairs();

    int connections = pairs.count;
    for (int i = 0; i < connections; i++) {
        BoxPair *pair = &pairs.pairs[i];
        log_debug("Connecting boxes\n");
        #ifdef DEBUG
            print_pair(pairs[i]);
        #endif
        connect_boxes(&boxes[pair->box1_idx], &boxes[pair->box2_idx]);
        if (all_connected()) {
            printf("%ld\n",
                   (long int) roundf(boxes[pair->box1_idx].x * boxes[pair->box2_idx].x));
            exit(0);
        }
    }

    fprintf(stderr, "ERROR: Multiple circuits remaining after connecting all boxes\n");

    return 1;
}
