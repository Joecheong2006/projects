#ifndef _MATRIX_H
#define _MATRIX_H

#include "util.h"

extern u64 memory_allocated;

struct matrix {
    f32** data;
    u64 col, row;
};

struct matrix* matrix_create(u64 col, u64 row);
struct matrix* matrix_add(struct matrix* m1, struct matrix* m2);
struct matrix* matrix_addc(struct matrix* m, f32 c);
struct matrix* matrix_subtract(struct matrix* m1, struct matrix* m2);
struct matrix* matrix_subtractc(struct matrix* m, f32 c);
struct matrix* matrix_multiply(struct matrix* m1, struct matrix* m2);
struct matrix* matrix_dot(struct matrix* m1, struct matrix* m2);
void matrix_set_rotateX(struct matrix* m, f32 radius);
void matrix_set_rotateY(struct matrix* m, f32 radius);
void matrix_set_rotateZ(struct matrix* m, f32 radius);
void matrix_apply(struct matrix* m, f32(*func)(f32));
void matrix_print(struct matrix* m);
void matrix_free(struct matrix* m);

void test_matrix_add(struct matrix* m1, struct matrix* m2);
void test_matrix_subtract(struct matrix* m1, struct matrix* m2);
void test_matrix_multiply(struct matrix* m1, struct matrix* m2);
void test_matrix_dot(struct matrix* m1, struct matrix* m2);
void test_matrix();

#endif
