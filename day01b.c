#include <stdio.h>
#include <stdlib.h>

// #define DEBUG

static char* DEFAULT_INPUT = "input/day01.txt";

int main(int argc, char **argv)
{
    FILE *file = fopen(argc >= 2 ? argv[1] : DEFAULT_INPUT, "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    int pos = 50;
    int password = 0;

    char dir;
    int n;
    while (fscanf(file, "%c%d\n", &dir, &n) >= 0) {
        if (dir == 'L') {
            n *= -1;
        }

        #ifdef DEBUG
            printf("pos = %3d; move = %4d; ", pos, n);
        #endif

        int quot = (pos + n) / 100;
        int zero_passes = abs(pos + n <= 0 && pos > 0 ? quot - 1 : quot);

        #ifdef DEBUG
            printf("zero_passes = %2d; ", zero_passes);
        #endif

        password += zero_passes;
        pos = (pos + n) % 100;
        if (pos < 0) {
            pos += 100;
        }

        #ifdef DEBUG
            printf("new_pos = %3d; password = %d;\n", pos, password);
        #endif
    }

    printf("%d\n", password);
    return 0;
}
