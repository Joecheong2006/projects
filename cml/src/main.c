#include <stdio.h>
#include <time.h>
#include "cml.h"

void print_vec3(vec3 v) {
    printf("%-6g %-6g %-6g\n", v[0], v[1], v[2]);
}

void print_vec4(vec4 v) {
    printf("%-6g %-6g %-6g %-6g\n", v[0], v[1], v[2], v[3]);
}

void print_mat3(mat3 m) {
    print_vec3(m[0]);
    print_vec3(m[1]);
    print_vec3(m[2]);
}

void print_mat4(mat4 m) {
    print_vec4(m[0]);
    print_vec4(m[1]);
    print_vec4(m[2]);
    print_vec4(m[3]);
}

void test_mat3(void) {
    {
        vec3 v = { 1, 2, 3 };
        mat3 a = {
            { 1, 0, 0 },
            { 0, 1, 1 },
            { 0, 1, 1 },
        };

        vec3 ov;

        printf("M:\n");
        print_mat3(a);
        printf("\nV:\n");
        print_vec3(v);

        mat3_mul_vec3(ov, a, v);
        printf("\nM x V:\n");
        print_vec3(ov);
    }

    {
        mat3 a = {
            { 10, 20, 10 },
            { 4, 5, 6 },
            { 2, 3, 5 },
        };
        mat3 b = {
            { 3, 2, 4 },
            { 3, 3, 9 },
            { 4, 4, 2 },
        };

        mat3 om;

        printf("A:\n");
        print_mat3(a);
        printf("\nB:\n");
        print_mat3(b);

        mat3_mul_mat3(om, a, b);
        printf("\nA x B:\n");
        print_mat3(om);
    }
}

void test_mat4(void) {
    {
        vec4 v = { 1, 2, 3, 4 };
        mat4 a = {
            { 1, 0, 0, 1 },
            { 0, 1, 1, 0 },
            { 0, 1, 1, 0 },
            { 1, 0, 0, 1 },
        };

        vec4 ov;

        printf("M:\n");
        print_mat4(a);
        printf("\nV:\n");
        print_vec4(v);

        mat4_mul_vec4(ov, a, v);
        printf("\nM x V:\n");
        print_vec4(ov);
    }

    {
        mat4 a = {
            { 5, 7, 9, 10 },
            { 2, 3, 3, 8 },
            { 8, 10, 2, 3 },
            { 3, 3, 4, 8 },
        };

        mat4 b = {
            { 3, 10, 12, 18 },
            { 12, 1, 4, 9 },
            { 9, 10, 12, 2 },
            { 3, 12, 4, 10 },
        };

        mat4 om;

        printf("\nA:\n");
        print_mat4(a);
        printf("\nB:\n");
        print_mat4(b);

        printf("\nA x B:\n");
        mat4_mul_mat4(om, a, b);
        print_mat4(om);
    }
}

#define PERFORMANCE_ITERATE 10000000

void test_mat3_performance(void) {
    vec3 v = { 1, 2, 3 };

    mat3 a = {
        { 10, 20, 10 },
        { 4, 5, 6 },
        { 2, 3, 5 },
    };

    mat3 b = {
       { 3, 2, 4 },
       { 3, 3, 9 },
       { 4, 4, 2 },
    };

    vec3 ov;
    mat3 om;

    clock_t start, end;

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat3f_mul_vec3f(ov, a, v);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat3f_mul_mat3f(om, a, b);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
}

void test_mat4_performance(void) {
    vec4 v = { 1, 2, 3, 4 };

    mat4 a = {
        { 5, 7, 9, 10 },
        { 2, 3, 3, 8 },
        { 8, 10, 2, 3 },
        { 3, 3, 4, 8 },
    };

    mat4 b = {
        { 3, 10, 12, 18 },
        { 12, 1, 4, 9 },
        { 9, 10, 12, 2 },
        { 3, 12, 4, 10 },
    };

    vec4 ov;
    mat4 om;

    clock_t start, end;

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat4f_mul_vec4f(ov, b, v);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat4f_mul_mat4f(om, a, b);
    }
    end = clock();
    printf("iterate: 10000000 takes: %dms\n", (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
}

typedef int a[1];

int main(void) {
    printf("[mat3 performance]\n");
    test_mat3_performance();
    printf("[mat4 performance]\n");
    test_mat4_performance();
}
