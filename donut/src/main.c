#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <windows.h>
#include "matrix.h"

#define clear() printf("\e[1;1H\e[2J");
#define disable_cursor() printf("\e[?25l");
#define enable_cursor() printf("\e[?25h");

#define PI 3.1415

#define SX 50
#define SY 50
static char buffer[SX * SY];

u64 memory_allocated;

void clear_buffer() {
    memset(buffer, ' ', SX * SY);
}

int main(void) {
    disable_cursor();

    float n = 1, f = 50;
    float t = tan(45.0f * PI / 180) * n, b = -t;
    float r = t * (SX / (float)SY), l = -r;

    struct matrix* projection = matrix_create(4, 4);
    projection->data[0][0] = 2 * n / (r - l);
    projection->data[0][2] = (r + l) / (r - l);
    projection->data[1][1] = 2 * n / (t - b);
    projection->data[1][2] = (t + b) / (t - b);
    projection->data[2][2] = -(f + n) / (f - n);
    projection->data[2][3] = -2 * f * n / (f - n);
    projection->data[3][2] = -1;

    float a = 0;
    struct matrix* rotateZ = matrix_create(4, 4);
    rotateZ->data[0][0] = cos(a);
    rotateZ->data[0][1] = -sin(a);
    rotateZ->data[1][0] = sin(a);
    rotateZ->data[1][1] = cos(a);
    rotateZ->data[2][2] = 1;

    struct matrix* pos = matrix_create(1, 4);
    pos->data[0][0] = 10;
    pos->data[1][0] = 0;
    pos->data[2][0] = -10;

    float ox = 25, oy = 25;

    while (1) {
        char c = getch();
        clear_buffer();
        switch (c) {
            case 'j': a -= 0.1; break;
            case 'k': a += 0.1; break;
            case 'w': oy -= 1; break;
            case 's': oy += 1; break;
            case 'a': ox -= 1; break;
            case 'd': ox += 1; break;
        }
        rotateZ->data[0][0] = cos(a);
        rotateZ->data[0][1] = -sin(a);
        rotateZ->data[1][0] = sin(a);
        rotateZ->data[1][1] = cos(a);
        struct matrix* ro_pos = matrix_dot(rotateZ, pos);
        struct matrix* new_pos = matrix_dot(projection, ro_pos);
        new_pos->data[0][0] += ox + 1;
        new_pos->data[1][0] += oy + 1;
        if (new_pos->data[0][0] * new_pos->data[1][0] >= 0)
            buffer[(int)new_pos->data[0][0] + (int)new_pos->data[1][0] * SX] = '#';
        buffer[(int)ox + (int)oy * SX] = 'X';
        matrix_free(new_pos);
        matrix_free(ro_pos);

        clear();

        for (int i = 0; i < SY; ++i) {
            for (int j = 0; j < SX; ++j) {
                putchar(buffer[j + i * SX]);
                putchar(' ');
            }
            putchar('\n');
        }
        if (c == 'q')
            break;
    }

    enable_cursor();
    matrix_free(projection);
    matrix_free(rotateZ);
    matrix_free(pos);

    printf("\n%llu", memory_allocated);
}

