#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/param.h>

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

static int cmpcircuit(const Circuit *c1, const Circuit *c2)
{
    if (c1->boxes_count > c2->boxes_count) {
        return -1;
    } else if (c1->boxes_count < c2->boxes_count) {
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

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input file> <connections>\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    int connections = atoi(argv[2]);

    assert(connections > 0 && "Connections must be a positive integer");

    parse_input(input_file);
    for (int i = 0; i < boxes_count; i++) {
        #ifdef DEBUG
            print_box(boxes[i]);
        #endif
    }

    BoxPairs pairs = build_box_pairs();

    connections = MIN(connections, pairs.count);
    for (int i = 0; i < connections; i++) {
        BoxPair *pair = &pairs.pairs[i];
        log_debug("Connecting boxes\n");
        #ifdef DEBUG
            print_pair(pairs[i]);
        #endif
        connect_boxes(&boxes[pair->box1_idx], &boxes[pair->box2_idx]);
    }

    qsort(circuits, (size_t) boxes_count, sizeof(Circuit), (int (*)(const void *, const void *)) cmpcircuit);

    long int product = 1;
    assert(boxes_count >= 3);
    for (int i = 0; i < 3; i++) {
        product *= circuits[i].boxes_count;
    }

    printf("%ld\n", product);

    return 0;
}
