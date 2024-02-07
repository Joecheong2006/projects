#include <stdio.h>
#include <string.h>
#include <math.h>
#include <conio.h>
#include <windows.h>
#include "matrix.h"

#define clear() printf("\e[1;1H\e[2J");
#define disable_cursor() printf("\e[?25l");
#define enable_cursor() printf("\e[?25h");

#define SX 50
#define SY 50
static char buffer[SX * SY];

u64 memory_allocated;

void clear_buffer() {
    memset(buffer, ' ', SX * SY);
}

int main(void) {
    disable_cursor();

    const float PI = acos(-1);

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

    struct matrix* rotateZ = matrix_create(4, 4);

    struct matrix* rotateY = matrix_create(4, 4);

    float xr = 7;
    float yr = 2;
    struct matrix* pos = matrix_create(1, 4);
    pos->data[0][0] = yr;
    pos->data[1][0] = 0;
    pos->data[2][0] = -3;
    //pos->data[2][0] = 1;

    float ox = (float)SX / 2, oy = (float)SY / 2;
    float angle_step = 0.1f;
    float angle_x = 0.1f;

    while (1) {
        char c = getch();
        if (c == 'q')
            break;
        clear_buffer();
        switch (c) {
            case 'a': ox -= 1; break;
            case 'd': ox += 1; break;
            case 'w': pos->data[2][0] -= 0.3; break;
            case 's': pos->data[2][0] += 0.3; break;
            case 'j': angle_x += 0.1; break;
            case 'k': angle_x -= 0.1; break;
            default:
                break;
        }

        for (float iy = 0; iy < 2 * PI; iy += angle_step) {
            rotateY->data[1][1] = 1;
            rotateY->data[0][0] = cosf(iy);
            rotateY->data[0][2] = sinf(iy);
            rotateY->data[2][0] = -sinf(iy);
            rotateY->data[2][2] = cosf(iy);
            for (float ix = 0; ix < 2 * PI; ix += angle_step) {
                rotateZ->data[2][2] = 1;
                rotateZ->data[0][0] = cosf(ix);
                rotateZ->data[0][1] = -sinf(ix);
                rotateZ->data[1][0] = sinf(ix);
                rotateZ->data[1][1] = cosf(ix);
                struct matrix* rz_pos = matrix_dot(rotateZ, pos);
                rz_pos->data[0][0] += xr;
                struct matrix* ry_pos = matrix_dot(rotateY, rz_pos);
                rotateZ->data[0][0] = 0;
                rotateZ->data[0][1] = 0;
                rotateZ->data[0][2] = 0;
                rotateZ->data[1][0] = 0;
                rotateZ->data[1][1] = 0;
                rotateZ->data[1][2] = 0;
                rotateZ->data[2][0] = 0;
                rotateZ->data[2][1] = 0;
                rotateZ->data[2][2] = 0;

                rotateZ->data[0][0] = 1;
                rotateZ->data[1][1] = cosf(angle_x);
                rotateZ->data[1][2] = -sinf(angle_x);
                rotateZ->data[2][1] = sinf(angle_x);
                rotateZ->data[2][2] = cosf(angle_x);
                struct matrix* rx_pos = matrix_dot(rotateZ, ry_pos);
                rotateZ->data[0][0] = 0;
                rotateZ->data[0][1] = 0;
                rotateZ->data[0][2] = 0;
                rotateZ->data[1][0] = 0;
                rotateZ->data[1][1] = 0;
                rotateZ->data[1][2] = 0;
                rotateZ->data[2][0] = 0;
                rotateZ->data[2][1] = 0;
                rotateZ->data[2][2] = 0;

                rotateZ->data[2][2] = 1;
                rotateZ->data[0][0] = cosf(angle_x / 2);
                rotateZ->data[0][1] = -sinf(angle_x / 2);
                rotateZ->data[1][0] = sinf(angle_x / 2);
                rotateZ->data[1][1] = cosf(angle_x / 2);
                struct matrix* ro_pos = matrix_dot(rotateZ, rx_pos);
                struct matrix* new_pos = matrix_dot(projection, ro_pos);

                float x = new_pos->data[0][0];
                float y = new_pos->data[1][0];
                float z = new_pos->data[2][0];
                if (new_pos->data[3][0] != 0 && x >= -ox && y >= -oy && x < SX && y < SY) {
                    if (buffer[(int)(x + ox) + (int)(y + oy) * SX] == ' ') {
                        buffer[(int)(x + ox) + (int)(y + oy) * SX] = '1' + (int)(z);
                    } else if(buffer[(int)(x + ox) + (int)(y + oy) * SX] < (int)z) {
                        buffer[(int)(x + ox) + (int)(y + oy) * SX] = '1' + (int)(z);
                    }
                }
                matrix_free(new_pos);
                matrix_free(rz_pos);
                matrix_free(ry_pos);
                matrix_free(rx_pos);
                matrix_free(ro_pos);
            }
        }

        clear();

        for (int i = 0; i < SY * SX; ++i) {
            putchar(i % SX ? buffer[i] : '\n');
            putchar(' ');
        }
    }

    enable_cursor();
    matrix_free(projection);
    matrix_free(rotateZ);
    matrix_free(rotateY);
    matrix_free(pos);

    printf("\n%llu", memory_allocated);
}

