#include "neuron.h"
#include <assert.h>
#include <math.h>

void neuron_init(struct neuron* n, u64 weight_count)
{
    assert(n != NULL);
    n->weight = matrix_create(1, weight_count);
    assert(n->weight != NULL);
}

void neuron_copy(struct neuron* n1, struct neuron* n2)
{
    assert(n1->weight->row == n2->weight->row && n1->weight->col == n2->weight->col);
    for(u64 i = 0; i < n1->weight-> row; i++)
        n1->weight->data[i][0] = n2->weight->data[i][0];
    n1->bias = n2->bias;
}

void neuron_free(struct neuron* n)
{
    assert(n != NULL);
    matrix_free(n->weight);
}

struct matrix* forward(struct neuron* n, struct matrix* input)
{
    struct matrix* result = matrix_dot(input, n->weight);
    for(u64 i = 0; i < result->row; i++)
        result->data[i][0] = result->data[i][0] + n->bias;
    return result;
}

f32 sigmoidf(f32 x)
{
    return 1 / (1 + expf(-x));
}

f32 tan_h(f32 x)
{
    f32 ep = expf(x);
    f32 en = expf(-x);
    return (ep - en) / (ep + en);
}

