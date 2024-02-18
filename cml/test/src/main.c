#include <stdio.h>
#include <time.h>
#include <cml.h>

#define PERFORMANCE_ITERATE (u64)10000000

void print_vec3(vec3 v) {
    printf("%-5g %-5g %-5g\n", v[0], v[1], v[2]);
}

void print_vec4(vec4 v) {
    printf("%-5g %-5g %-5g %-5g\n", v[0], v[1], v[2], v[3]);
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
        vec3 v = { 2, 5, 1 };
        printf("\nV:\n");
        print_vec3(v);

        printf("\nV normalized:\n");
        vec3_normalize(v);
        print_vec3(v);
    }
    {
        vec3 u = { 1, 2, 3 };
        vec3 v = { 2, 5, 1 };

        printf("\nU:\n");
        print_vec3(u);
        printf("\nV:\n");
        print_vec3(v);

        printf("\nU * V:\n");
        printf("%-6g\n", vec3_dot(u, v));
    }
    {
        vec3 u = { 1, 2, 3 };
        vec3 v = { 5, 0, -1 };

        printf("\nU:\n");
        print_vec3(u);
        printf("\nV:\n");
        print_vec3(v);

        vec3_cross(v, u, v);
        printf("\nU x V:\n");
        print_vec3(v);
    }
    {
        vec3 v = { 1, 2, 3 };
        mat3 a = {
            { 1, 0, 0 },
            { 0, 1, 1 },
            { 0, 1, 1 },
        };

        vec3 ov;

        printf("\nM:\n");
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

        printf("\nA:\n");
        print_mat3(a);
        printf("\nB:\n");
        print_mat3(b);

        mat3_mul(om, a, b);
        printf("\nA x B:\n");
        print_mat3(om);
    }
}

void test_mat4(void) {
    {
        mat4 m = MAT4_SET(1);
        printf("\nM seted[1]:\n");
        print_mat4(m);
        mat4_translate(m, m, (vec3){ 3, 4, 2 });
        mat4_scale(m, m, (vec3){ 1, 2, 1 });
        mat4_scale(m, m, (vec3){ 2, 3, 8 });
        printf("\nM translated[3, 4, 2] scaled[1, 2, 1][2, 3, 8]:\n");
        print_mat4(m);
    }
    {
        vec4 v = { 2, 5, 1, 3 };
        printf("\nV:\n");
        print_vec4(v);

        printf("\nV normalized:\n");
        vec4_normalize(v);
        print_vec4(v);
    }
    {
        vec4 u = { 1, 2, 3, 4 };
        vec4 v = { 2, 5, 1, 3 };

        printf("\nU:\n");
        print_vec4(u);
        printf("\nV:\n");
        print_vec4(v);

        printf("\nU * V:\n");
        printf("%-6g\n", vec4_dot(u, v));
    }
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
        mat4_mul(om, a, b);
        print_mat4(om);
    }
}


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

    clock_t start, end;

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat3_mul_vec3(v, a, v);
    }
    end = clock();
    printf("iterate: %llu takes: %dms\n", PERFORMANCE_ITERATE, (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
    print_vec3(v);

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat3_mul(a, a, b);
    }
    end = clock();
    printf("iterate: %llu takes: %dms\n", PERFORMANCE_ITERATE, (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
    print_mat3(a);
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

    clock_t start, end;

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat4_mul_vec4(v, b, v);
    }
    end = clock();
    printf("iterate: %llu takes: %dms\n", PERFORMANCE_ITERATE, (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
    print_vec4(v);

    start = clock();
    for (u64 i = 0; i < PERFORMANCE_ITERATE; ++i) {
        mat4_mul(a, a, b);
    }
    end = clock();
    printf("iterate: %llu takes: %dms\n", PERFORMANCE_ITERATE, (i32)(1000 * (end - start) / CLOCKS_PER_SEC));
    print_mat4(a);
}

void set_projection(mat4 proj, f32 fov, f32 aspect, f32 n, f32 f) {
    f32 t = tanf(fov * 0.5);
    mat4_zero(proj);
    proj[0][0] = 1 / (t * aspect);
    proj[1][1] = 1 / t;
    proj[2][2] = -(f + n) / (f - n);
    proj[2][3] = -2 * f * n / (f - n);
    proj[3][2] = -1;
}

int main(void) {
    //test_mat3();
    //test_mat4();

    printf("[mat3 performance]\n");
    test_mat3_performance();
    printf("[mat4 performance]\n");
    test_mat4_performance();
}
