#include <stdio.h>

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
        pos = (pos + n) % 100;
        if (pos == 0) {
            password++;
        }
    }

    printf("%d\n", password);
    return 0;
}
