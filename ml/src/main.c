#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrix.h"
u64 memory_allocated;
#include "neuron.h"
#include "csv.h"

#include <math.h>

#define COL 3
#define ROW 4
#define INPUT_COUNT 2
#define OUTPUT_COUNT 1

f32  train_data[ROW][COL] =
{
    {0, 0, 0},
    {1, 0, 1},
    {0, 1, 1},
    {1, 1, 1},
};

f32 mse(struct matrix* input, struct matrix* output, struct neuron* n)
{
    float result = 0;
    struct matrix* y = matrix_dot(input, n->weight);
    matrix_addc(y, n->bias);
    matrix_apply(y, tan_h);
    struct matrix* d = matrix_subtract(output, y);
    for(u64 i = 0; i < d->row; i++)
        result += d->data[i][0] * d->data[i][0];
    result /= d->row;
    matrix_free(y);
    matrix_free(d);
    return result;
}

void print_train_result(struct matrix* input, struct neuron* n)
{
    struct matrix* y = forward(n, input);
    matrix_apply(y, tan_h);
    matrix_print(y);
    matrix_free(y);
}

f32 rand_float()
{
    return (f32)rand() / RAND_MAX;
}

struct neuron train_neuron(struct matrix* input, struct matrix* output, struct neuron* n, f32 rate)
{
    struct neuron result;
    neuron_init(&result, n->weight->row);
    for(u64 i = 0; i < n->weight-> row; i++)
        result.weight->data[i][0] = n->weight->data[i][0];
    result.bias = n->bias;

    struct matrix* y = matrix_dot(input, n->weight);
    matrix_addc(y, n->bias);
    matrix_apply(y, tan_h);
    struct matrix* d = matrix_subtract(output, y);
    f32 c = rate * 2.0f / d->row;

    for(u64 offset = 0; offset < n->weight->row; offset++)
        for(u64 i = 0; i < d->row; i++)
            result.weight->data[offset][0] += c * d->data[i][0] * input->data[i][offset];

    for(u64 i = 0; i < d->row; i++)
        result.bias  += c * d->data[i][0];

    matrix_free(y);
    matrix_free(d);

    //for(u64 i = 0; i < n->weight->row; i++)
    //    result.weight->data[i][0] -= pd(input, output, n, i) * rate;
    //result.bias -= pb(input, output, n) * rate;

    return result;
}

i32 main(void)
{
    srand(time(0));

    f32 rate = 1e-2;
    u32 train_count = 100 * 100;

    struct matrix* input = matrix_create(INPUT_COUNT, ROW);
    struct matrix* output = matrix_create(OUTPUT_COUNT, ROW);

    for(u64 i = 0; i < input->row; i++)
        for(u64 j = 0; j < input->col; j++)
            input->data[i][j] = train_data[i][j];
    for(u64 i = 0; i < output->row; i++)
        output->data[i][0] = train_data[i][input->col];

    struct neuron n1;
    neuron_init(&n1, INPUT_COUNT);
    for(u64 i = 0; i < n1.weight->row; i++)
        n1.weight->data[i][0] = rand_float() * 10;
    n1.bias = rand_float() * 5;

    for(u64 i = 0; i < train_count; i++)
    {
        struct neuron result = train_neuron(input, output, &n1, rate);
        neuron_copy(&n1, &result);
        neuron_free(&result);
        printf("%f\n", mse(input, output, &n1));
    }

    print_train_result(input, &n1);

    matrix_free(input);
    matrix_free(output);
    neuron_free(&n1);

    printf("memory allocated = %lld", memory_allocated);
    return 0;
}
