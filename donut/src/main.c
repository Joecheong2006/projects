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

#define SX 60
#define SY 25
char buffer[SX * SY];
char dbuffer[SX * SY];
char sc = '.';

u64 memory_allocated;

float ox = (float)SX / 2, oy = (float)SY / 2, fov = 60;
float angle_step = 0.1f;
float angle_x = 0.1f;
float angle_delta = 0.1;
struct matrix* pos;
struct matrix* cam;
struct matrix* lpos;
int looping = 1;

void* input_handle() {
    while (1) {
        char c = getch();
        if (c == 'q') {
            looping = 0;
            return NULL;
        }
        switch (c) {
            case 'a': cam->data[0][0] -= 0.05; break;
            case 'd': cam->data[0][0] += 0.05; break;
            case 'w': cam->data[2][0] -= 0.05; break;
            case 's': cam->data[2][0] += 0.05; break;
            case 'j': angle_delta += 0.01; break;
            case 'k': angle_delta -= 0.01; break;
            case 'u': angle_step += 0.01; break;
            case 'i': angle_step -= 0.01; break;
            case 'c': fov += 1; break;
            case 'v': fov -= 1; break;
            default:
                break;
        }
    }

    return NULL;
}

void set_projection(struct matrix* proj, float n, float f, float fov, float aspect) {
    const float PI180 = acosf(-1) / 180;
    float t = tan(fov * PI180) * n;
    float r = t * aspect;
    proj->data[0][0] = n / r;
    proj->data[0][1] = 0;
    proj->data[0][2] = 0;
    proj->data[0][3] = 0;

    proj->data[1][0] = 0;
    proj->data[1][1] = n / t;
    proj->data[1][2] = 0;
    proj->data[1][4] = 0;

    proj->data[2][0] = 0;
    proj->data[2][1] = 0;
    proj->data[2][2] = -(f + n) / (f - n);
    proj->data[2][3] = -2 * f * n / (f - n);

    proj->data[3][0] = 0;
    proj->data[3][1] = 0;
    proj->data[3][2] = -1;
    proj->data[3][3] = 0;
}

void normalize_vec3(struct matrix* v) {
    float r = sqrtf(v->data[0][0] * v->data[0][0] + v->data[1][0] * v->data[1][0] + v->data[2][0] * v->data[2][0]);
    v->data[0][0] /= r;
    v->data[1][0] /= r;
    v->data[2][0] /= r;
}

int main(void) {
    disable_cursor();

    const float PI = acos(-1);
    float n = 0.1, f = 20;

    struct matrix* M = matrix_create(4, 4);
    struct matrix* rotateY = matrix_create(4, 4);

    float xr = 0.8;
    float yr = 0.4;

    pos = matrix_create(1, 4);
    pos->data[0][0] = yr;
    pos->data[1][0] = 0;
    pos->data[2][0] = 0;
    pos->data[3][0] = 0;

    cam = matrix_create(1, 3);
    cam->data[0][0] = 0;
    cam->data[1][0] = 0;
    cam->data[2][0] = 2;

    lpos = matrix_create(1, 3);
    lpos->data[0][0] = 0;
    lpos->data[1][0] = 1;
    lpos->data[2][0] = -1;
    normalize_vec3(lpos);

    pthread_t input_thread;
    pthread_create(&input_thread, NULL, input_handle, NULL);

    clear();
    while (looping) {
        memset(buffer, ' ', SX * SY);
        memset(dbuffer, 0, SX * SY);

        for (float iy = 0; iy < 2 * PI; iy += angle_step * 0.5) {
            matrix_set_rotateY(rotateY, iy);
            for (float ix = 0; ix < 2 * PI; ix += angle_step) {
                matrix_set_rotateZ(M, ix);
                struct matrix* rz_pos = matrix_dot(M, pos);
                struct matrix* rz_normal = matrix_create(0, 4);
                rz_normal->data[0][0] = cosf(ix);
                rz_normal->data[1][0] = sinf(ix);
                rz_normal->data[2][0] = 0; 
                rz_normal->data[3][0] = 0; 

                rz_pos->data[0][0] += xr;
                struct matrix* ry_pos = matrix_dot(rotateY, rz_pos);
                struct matrix* ry_normal = matrix_dot(rotateY, rz_normal);

                matrix_set_rotateX(M, angle_x);
                struct matrix* rx_pos = matrix_dot(M, ry_pos);
                struct matrix* rx_normal = matrix_dot(M, ry_normal);
                matrix_set_rotateZ(M, angle_x);
                struct matrix* ro_pos = matrix_dot(M, rx_pos);
                struct matrix* ro_normal = matrix_dot(M, rx_normal);

                ro_pos->data[0][0] -= cam->data[0][0];
                ro_pos->data[1][0] -= cam->data[1][0];
                ro_pos->data[2][0] -= cam->data[2][0];
                ro_normal->data[0][0] -= cam->data[0][0];
                ro_normal->data[1][0] -= cam->data[1][0];
                ro_normal->data[2][0] -= cam->data[2][0];

                set_projection(M, n, f, fov, SX / (float)(SY * 2.3));
                struct matrix* new_pos = matrix_dot(M, ro_pos);

                normalize_vec3(ro_normal);
                float normal = ro_normal->data[0][0] * lpos->data[0][0] + ro_normal->data[1][0] * lpos->data[1][0] + ro_normal->data[2][0] * lpos->data[2][0];

                float w = new_pos->data[3][0];
                float x = SX * new_pos->data[0][0] / w;
                float y = SY * new_pos->data[1][0] / w;
                float z = w / new_pos->data[2][0];

                if (normal > 0) {
                    int index = (int)(x + SX * 0.5f) + (int)(y + SY * 0.5f) * SX;
                    if (x < SX * 0.5f && x > -SX * 0.5f && y < SY * 0.5f && y > -SY * 0.5f && dbuffer[index] < z) {
                        buffer[index] = ".,-~:;=!*#$@"[(int)(normal * 11)];
                        dbuffer[index] = z;
                    }
                }

                matrix_free(new_pos);
                matrix_free(rz_pos);
                matrix_free(ry_pos);
                matrix_free(rx_pos);
                matrix_free(ro_pos);
                matrix_free(rz_normal);
                matrix_free(ry_normal);
                matrix_free(rx_normal);
                matrix_free(ro_normal);
            }
        }

        angle_x += angle_delta;

        gotoxy(0, 0);
        for (int i = 0; i < SY * SX; ++i) {
            putchar(i % SX ? buffer[i] : '\n');
        }
    }
    clear();

    pthread_join(input_thread, NULL);

    enable_cursor();
    matrix_free(M);
    matrix_free(rotateY);
    matrix_free(pos);
    matrix_free(cam);
    matrix_free(lpos);

    printf("\n%llu", memory_allocated);
}

