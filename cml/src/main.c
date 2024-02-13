#include <stdio.h>
#include <time.h>
#include "cml.h"

void print_vec3(vec3* v) {
    printf("%-6g %-6g %-6g\n", v->x, v->y, v->z);
}

void print_vec4(vec4* v) {
    printf("%-6g %-6g %-6g %-6g\n", v->x, v->y, v->z, v->w);
}

void print_mat3(mat3* m) {
    print_vec3(&m->col[0]);
    print_vec3(&m->col[1]);
    print_vec3(&m->col[2]);
}

void print_mat4(mat4* m) {
    print_vec4(&m->col[0]);
    print_vec4(&m->col[1]);
    print_vec4(&m->col[2]);
    print_vec4(&m->col[3]);
}

void test_mat3(void) {
    vec3 v = INIT_VEC3(1, 2, 3);
    mat3 a, b;

    a.col[0] = INIT_VEC3(1, 0, 0);
    a.col[1] = INIT_VEC3(0, 1, 1);
    a.col[2] = INIT_VEC3(0, 1, 1);

    printf("M:\n");
    print_mat3(&a);
    printf("\nV:\n");
    print_vec3(&v);

    mat3_mul_vec3(&v, &a, &v);
    printf("\nM x V:\n");
    print_vec3(&v);

    a.col[0] = INIT_VEC3(10, 20, 10);
    a.col[1] = INIT_VEC3(4, 5, 6);
    a.col[2] = INIT_VEC3(2, 3, 5);

    b.col[0] = INIT_VEC3(3, 2, 4);
    b.col[1] = INIT_VEC3(3, 3, 9);
    b.col[2] = INIT_VEC3(4, 4, 2);

    printf("A:\n");
    print_mat3(&a);
    printf("\nB:\n");
    print_mat3(&b);

    mat3_mul_mat3(&b, &a, &b);
    printf("\nA x B:\n");
    print_mat3(&b);
}

void test_mat4(void) {
    vec4 v = INIT_VEC4(1, 2, 3, 4);
    mat4 a, b;

    a.col[0] = INIT_VEC4(1, 0, 0, 1);
    a.col[1] = INIT_VEC4(0, 1, 1, 0);
    a.col[2] = INIT_VEC4(0, 1, 1, 0);
    a.col[3] = INIT_VEC4(1, 0, 0, 1);

    printf("M:\n");
    print_mat4(&a);
    printf("\nV:\n");
    print_vec4(&v);

    mat4_mul_vec4(&v, &a, &v);
    printf("\nM x V:\n");
    print_vec4(&v);

    a.col[0] = INIT_VEC4(5, 7, 9, 10);
    a.col[1] = INIT_VEC4(2, 3, 3, 8);
    a.col[2] = INIT_VEC4(8, 10, 2, 3);
    a.col[3] = INIT_VEC4(3, 3, 4, 8);

    b.col[0] = INIT_VEC4(3, 10, 12, 18);
    b.col[1] = INIT_VEC4(12, 1, 4, 9);
    b.col[2] = INIT_VEC4(9, 10, 12, 2);
    b.col[3] = INIT_VEC4(3, 12, 4, 10);

    printf("\nA:\n");
    print_mat4(&a);
    printf("\nB:\n");
    print_mat4(&b);

    printf("\nA x B:\n");
    mat4_mul_mat4(&b, &a, &b);
    print_mat4(&b);
}

#define PERFORMANCE_ITERATE 10000000

void test_mat3_performance(void) {
    vec3 v = INIT_VEC3(1, 2, 3), vo;
    mat3 a, b, mo;

    a.col[0] = INIT_VEC3(10, 20, 10);
    a.col[1] = INIT_VEC3(4, 5, 6);
    a.col[2] = INIT_VEC3(2, 3, 5);

    b.col[0] = INIT_VEC3(3, 2, 4);
    b.col[1] = INIT_VEC3(3, 3, 9);
    b.col[2] = INIT_VEC3(4, 4, 2);

    clock_t start, end;

    start = clock();
    for (i32 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat3f_mul_vec3f(&vo, &a, &v);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));

    start = clock();
    for (i32 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat3f_mul_mat3f(&mo, &a, &b);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
}

void test_mat4_performance(void) {
    vec4 v = INIT_VEC4(1, 2, 3, 4), vo;
    mat4 a, b, mo;

    a.col[0] = INIT_VEC4(5, 7, 9, 10);
    a.col[1] = INIT_VEC4(2, 3, 3, 8);
    a.col[2] = INIT_VEC4(8, 10, 2, 3);
    a.col[3] = INIT_VEC4(3, 3, 4, 8);

    b.col[0] = INIT_VEC4(3, 10, 12, 18);
    b.col[1] = INIT_VEC4(12, 1, 4, 9);
    b.col[2] = INIT_VEC4(9, 10, 12, 2);
    b.col[3] = INIT_VEC4(3, 12, 4, 10);

    clock_t start, end;

    start = clock();
    for (i32 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat4f_mul_vec4f(&vo, &b, &v);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));

    start = clock();
    for (i32 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat4f_mul_mat4f(&mo, &a, &b);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
}

typedef int a[1];

int main(void) {
    //test_mat3();
    //test_mat4();
    printf("[mat3 performance]\n");
    test_mat3_performance();
    printf("[mat4 performance]\n");
    test_mat4_performance();
}
