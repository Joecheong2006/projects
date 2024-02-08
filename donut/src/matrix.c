#include "matrix.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

struct matrix* matrix_create(u64 col, u64 row)
{
    struct matrix* m = (struct matrix*)malloc(sizeof(struct matrix));
    f32** rows = (f32**)malloc(sizeof(f32*) * row);
    assert(rows != NULL);

    m->data = rows;
    m->row = row;
    m->col = col;

    for(u64 row = 0; row < m->row; row++)
    {
        f32* cols = (f32*)malloc(sizeof(f32) * col);
        assert(cols != NULL);
        m->data[row] = cols;
    }

    memory_allocated += (sizeof(struct matrix) + sizeof(f32*) * m->row + sizeof(f32) * m->col);
    return m;
}

struct matrix* matrix_add(struct matrix* m1, struct matrix* m2)
{
    assert(m1->row == m2->row && m1->col == m2->col);
    struct matrix* m = matrix_create(m1->col, m1->row);
    for(u64 row = 0; row < m1->row; row++)
    {
        for(u64 col = 0; col < m2->col; col++)
        {
            m->data[row][col] = m1->data[row][col] + m2->data[row][col];
        }
    }
    return m;
}

struct matrix* matrix_addc(struct matrix* m, f32 c)
{
    for(u64 row = 0; row < m->row; row++)
    {
        for(u64 col = 0; col < m->col; col++)
        {
            m->data[row][col] += c;
        }
    }
    return m;
}

struct matrix* matrix_subtract(struct matrix* m1, struct matrix* m2)
{
    assert(m1->row == m2->row && m1->col == m2->col);
    struct matrix* m = matrix_create(m1->col, m1->row);
    for(u64 row = 0; row < m1->row; row++)
    {
        for(u64 col = 0; col < m2->col; col++)
        {
            m->data[row][col] = m1->data[row][col] - m2->data[row][col];
        }
    }
    return m;
}

struct matrix* matrix_subtractc(struct matrix* m, f32 c)
{
    return matrix_addc(m, -c);
}

struct matrix* matrix_multiply(struct matrix* m1, struct matrix* m2)
{
    assert(m1->col == m2->col && m1->row == m2->row);
    struct matrix* m = matrix_create(m1->col, m1->row);
    for(u64 row = 0; row < m1->row; row++)
    {
        for(u64 col = 0; col < m2->col; col++)
        {
            m->data[row][col] = m1->data[row][col] * m2->data[row][col];
        }
    }
    return m;
}

struct matrix* matrix_dot(struct matrix* m1, struct matrix* m2)
{
    assert(m1->col == m2->row);
    struct matrix* m = matrix_create(m2->col, m1->row);
    for(u64 row = 0; row < m1->row; row++)
    {
        for(u64 col = 0; col < m2->col; col++)
        {
            m->data[row][col] = 0;
            for(u64 scol = 0; scol < m1->col; scol++)
            {
                m->data[row][col] += m1->data[row][scol] * m2->data[scol][col];
            }
        }
    }
    return m;
}

void matrix_set_rotateX(struct matrix* m, f32 radius) {
    assert(m->col == 4 && m->row == 4);
    m->data[0][0] = 1;
    m->data[0][1] = 0;
    m->data[0][2] = 0;
    m->data[1][0] = 0;
    m->data[1][1] = cosf(radius);
    m->data[1][2] = -sinf(radius);
    m->data[2][0] = 0;
    m->data[2][1] = sinf(radius);
    m->data[2][2] = cosf(radius);
}

void matrix_set_rotateY(struct matrix* m, f32 radius) {
    assert(m->col == 4 && m->row == 4);
    m->data[0][0] = cosf(radius);
    m->data[0][1] = 0;
    m->data[0][2] = sinf(radius);
    m->data[1][0] = 0;
    m->data[1][1] = 1;
    m->data[1][2] = 0;
    m->data[2][0] = -sinf(radius);
    m->data[2][1] = 0;
    m->data[2][2] = cosf(radius);
}

void matrix_set_rotateZ(struct matrix* m, f32 radius) {
    assert(m->col == 4 && m->row == 4);
    m->data[0][0] = cosf(radius);
    m->data[0][1] = -sinf(radius);
    m->data[0][2] = 0;
    m->data[1][0] = sinf(radius);
    m->data[1][1] = cosf(radius);
    m->data[1][2] = 0;
    m->data[2][0] = 0;
    m->data[2][1] = 0;
    m->data[2][2] = 1;
}

void matrix_apply(struct matrix* m, f32(*func)(f32))
{
    for(u64 row = 0; row < m->row; row++)
        for(u64 col = 0; col < m->col; col++)
            m->data[row][col] = func(m->data[row][col]);
}

void matrix_print(struct matrix* m)
{
    assert(m != NULL);
    for(u64 row = 0; row < m->row; row++)
    {
        for(u64 col = 0; col < m->col; col++)
            printf("%6.2f ", m->data[row][col]);
        printf("\n");
    }
}

void matrix_free(struct matrix* m)
{
    assert(m != NULL);
    memory_allocated -= (sizeof(struct matrix) + sizeof(f32*) * m->row + sizeof(f32) * m->col);
    for(u64 row = 0; row < m->row; row++)
        free(m->data[row]);
    free(m->data);
    free(m);
}

void test_matrix_add(struct matrix* m1, struct matrix* m2)
{
    printf("Matrix Addition\n");

    printf("------ m1 --------\n");
    matrix_print(m1);
    printf("------ m2 --------\n");
    matrix_print(m2);

    struct matrix* m3 = matrix_add(m1, m2);

    printf("---- m1 + m2 -----\n");
    matrix_print(m3);

    matrix_free(m3);
}

void test_matrix_subtract(struct matrix* m1, struct matrix* m2)
{
    printf("Matrix Subtraction\n");

    printf("------ m1 --------\n");
    matrix_print(m1);
    printf("------ m2 --------\n");
    matrix_print(m2);

    struct matrix* m3 = matrix_subtract(m1, m2);

    printf("---- m1 - m2 -----\n");
    matrix_print(m3);

    matrix_free(m3);
}

void test_matrix_multiply(struct matrix* m1, struct matrix* m2)
{
    printf("Test Matrix Multiplication\n");

    printf("------ m1 --------\n");
    matrix_print(m1);
    printf("------ m2 --------\n");
    matrix_print(m2);

    struct matrix* m3 = matrix_multiply(m1, m2);

    printf("---- m1 * m2 -----\n");
    matrix_print(m3);

    matrix_free(m3);
}

void test_matrix_dot(struct matrix* m1, struct matrix* m2)
{
    printf("Test Matrix Dot Product\n");

    printf("------ m1 --------\n");
    matrix_print(m1);
    printf("------ m2 --------\n");
    matrix_print(m2);

    struct matrix* m3 = matrix_dot(m1, m2);

    printf("---- m1 x m2 -----\n");
    matrix_print(m3);

    matrix_free(m3);
}

void test_matrix()
{
    f32 d1[2][3] = {
        {1, 2, 3},
        {4, 5, 6},
    };

    f32 d2[3][2] = {
        {7, 8},
        {9, 10},
        {11, 12},
    };

    f32 asm1[3][3] = {
        {1, 2, 3},
        {7, 8, 9},
        {4, 5, 6},
    };

    f32 asm2[3][3] = {
        {4, 5, 6},
        {1, 2, 3},
        {7, 8, 9},
    };

    struct matrix* m1 = matrix_create(3, 3);
    struct matrix* m2 = matrix_create(3, 3);

    for(u64 row = 0; row < m1->row; row++)
        for(u64 col = 0; col < m1->col; col++)
            m1->data[row][col] = asm1[row][col];

    for(u64 row = 0; row < m2->row; row++)
        for(u64 col = 0; col < m2->col; col++)
            m2->data[row][col] = asm2[row][col];

    test_matrix_add(m1, m2);
    test_matrix_subtract(m1, m2);
    test_matrix_multiply(m1, m2);

    matrix_free(m1);
    matrix_free(m2);

    m1 = matrix_create(3, 2);
    m2 = matrix_create(2, 3);

    for(u64 row = 0; row < m1->row; row++)
        for(u64 col = 0; col < m1->col; col++)
            m1->data[row][col] = d1[row][col];

    for(u64 row = 0; row < m2->row; row++)
        for(u64 col = 0; col < m2->col; col++)
            m2->data[row][col] = d2[row][col];

    test_matrix_dot(m1, m2);

    matrix_free(m1);
    matrix_free(m2);
}
