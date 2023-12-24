#ifndef _NEURON_H
#define _NEURON_H

#include "util.h"
#include "matrix.h"

struct neuron
{
    struct matrix* weight;
    f32 bias;
};

void neuron_init(struct neuron* n, u64 weight_count);
void neuron_copy(struct neuron* n1, struct neuron* n2);
void neuron_free(struct neuron* n);
struct matrix* forward(struct neuron* n, struct matrix* input);

f32 sigmoidf(f32 x);
f32 tan_h(f32 x);
 
#endif
