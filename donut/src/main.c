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

#define SX 40
#define SY 40
char buffer[SX * SY];
char sc = 'A';

u64 memory_allocated;

float ox = (float)SX / 2, oy = (float)SY / 2, oz;
float angle_step = 0.1f;
float angle_x = 0.1f;
float angle_delta = 0.1;
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
            case 'w': oz -= 0.3; break;
            case 's': oz += 0.3; break;
            case 'j': angle_delta += 0.01; break;
            case 'k': angle_delta -= 0.01; break;
            default:
                break;
        }
    }

    return NULL;
}

void set_projection(struct matrix* proj, float n, float f, float fov, float rate) {
    const float PI = acos(-1);
    float t = tan(fov * 0.5 * PI / 180) * n, b = -t;
    float r = t * rate, l = -r;
    proj->data[0][0] = 2 * n / (r - l);
    proj->data[0][1] = 0;
    proj->data[0][2] = (r + l) / (r - l);
    proj->data[0][3] = 0;

    proj->data[1][0] = 0;
    proj->data[1][1] = 2 * n / (t - b);
    proj->data[1][2] = (t + b) / (t - b);
    proj->data[1][4] = 0;

    proj->data[2][0] = 0;
    proj->data[2][1] = 0;
    proj->data[2][2] = -(f + n) / (f - n);
    proj->data[2][3] = -2 * f * n / (f - n);

    proj->data[3][0] = 0;
    proj->data[3][1] = 0;
    proj->data[3][2] = 1;
    proj->data[3][3] = 0;
}

int main(void) {
    disable_cursor();

    const float PI = acos(-1);
    float n = 0.1f, f = 50;

    struct matrix* projection = matrix_create(4, 4);
    set_projection(projection, n, f, 90, SX / (float)SY);

    struct matrix* rotateZ = matrix_create(4, 4);
    struct matrix* rotateY = matrix_create(4, 4);

    float xr = 8;
    float yr = 4;

    pos = matrix_create(1, 4);
    pos->data[0][0] = yr;
    pos->data[1][0] = 0;
    pos->data[2][0] = 0;
    pos->data[3][0] = 1;

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

                ry_pos->data[1][0] += 15;
                matrix_set_rotateX(rotateZ, angle_x);
                struct matrix* rx_pos = matrix_dot(rotateZ, ry_pos);
                matrix_set_rotateZ(rotateZ, angle_x * 0.2);
                struct matrix* ro_pos = matrix_dot(rotateZ, rx_pos);
                ro_pos->data[2][0] += oz;
                struct matrix* new_pos = matrix_dot(projection, ro_pos);

                if (new_pos->data[3][0] != 0) {
                    float x = new_pos->data[0][0];
                    float y = new_pos->data[1][0];
                    int z = new_pos->data[2][0];
                    int index = (int)(x + ox - 1) + (int)(y + oy - 1) * SX;
                    if (index > 0 && index < SX * SY && x < SX - ox && x > -ox) {
                        if (buffer[index] == ' ') {
                            buffer[index] = sc + z;
                        } else if(buffer[index] < z) {
                            buffer[index] = sc + z;
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

        angle_x += angle_delta;

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

