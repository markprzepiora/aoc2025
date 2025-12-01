#include <stdio.h>

int main(void) {
    FILE *file = fopen("input/day01.txt", "r");
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
