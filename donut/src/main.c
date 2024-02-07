#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <pthread.h>
#include <windows.h>
#include "matrix.h"

#define clear() printf("\e[1;1H\e[2J");
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))
#define disable_cursor() printf("\e[?25l");
#define enable_cursor() printf("\e[?25h");

#define SX 50
#define SY 50
char buffer[SX * SY];
char sc = '9';

u64 memory_allocated;

float ox = (float)SX / 2, oy = (float)SY / 2, oz;
float angle_step = 0.1f;
float angle_x = 0.1f;
struct matrix* pos;
int looping = 1;

void* input_handle() {
    while (1) {
        char c = getch();
        if (c == 'q') {
            looping = 0;
            return NULL;
        }
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
    }

    return NULL;
}

int main(void) {
    disable_cursor();

    const float PI = acos(-1);

    float n = 0.1f, f = 50;
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

    float xr = 8;
    float yr = 3;
    pos = matrix_create(1, 4);
    pos->data[0][0] = yr;
    pos->data[1][0] = 0;
    pos->data[2][0] = 0;

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input_handle, NULL);

    clear();
    while (looping) {
        memset(buffer, ' ', SX * SY);

        for (float iy = 0; iy < 2 * PI; iy += angle_step) {
            matrix_set_rotateY(rotateY, iy);
            for (float ix = 0; ix < 2 * PI; ix += angle_step) {
                matrix_set_rotateZ(rotateZ, ix);
                struct matrix* rz_pos = matrix_dot(rotateZ, pos);
                rz_pos->data[0][0] += xr;
                struct matrix* ry_pos = matrix_dot(rotateY, rz_pos);
                matrix_set_rotateX(rotateZ, angle_x);
                struct matrix* rx_pos = matrix_dot(rotateZ, ry_pos);
                matrix_set_rotateZ(rotateZ, angle_x * 0.4);
                struct matrix* ro_pos = matrix_dot(rotateZ, rx_pos);
                rz_pos->data[3][0] += 10;
                struct matrix* new_pos = matrix_dot(projection, ro_pos);

                if (new_pos->data[3][0] != 0) {
                    float w = new_pos->data[3][0];
                    float x = new_pos->data[0][0];
                    float y = new_pos->data[1][0];
                    float z = new_pos->data[2][0];
                    if (x > -ox && y > -oy && x < SX && y < SY) {
                        if (buffer[(int)(x + ox) + (int)(y + oy) * SX] == ' ') {
                            buffer[(int)(x + ox) + (int)(y + oy) * SX] = sc + (int)(z);
                        } else if(buffer[(int)(x + ox) + (int)(y + oy) * SX] < (int)z) {
                            buffer[(int)(x + ox) + (int)(y + oy) * SX] = sc + (int)(z);
                        }
                    }
                }

                matrix_free(new_pos);
                matrix_free(rz_pos);
                matrix_free(ry_pos);
                matrix_free(rx_pos);
                matrix_free(ro_pos);
            }
        }

        angle_x += 0.2;

        gotoxy(0, 0);
        for (int i = 0; i < SY * SX; ++i) {
            putchar(i % SX ? buffer[i] : '\n');
            putchar(' ');
        }
        //Sleep(1);
    }
    clear();

    pthread_join(input_thread, NULL);

    enable_cursor();
    matrix_free(projection);
    matrix_free(rotateZ);
    matrix_free(rotateY);
    matrix_free(pos);

    printf("\n%llu", memory_allocated);
}

