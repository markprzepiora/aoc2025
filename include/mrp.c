#include <stdio.h>
#include <stdlib.h>

FILE *open_input_or_die(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }
    return file;
}
