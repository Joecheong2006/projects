#ifndef _STACK_H_
#define _STACK_H_
#include "container/vector.h"
#include "primitive_data.h"

typedef vector(primitive_data) stack;
#define make_stack() make_vector(primitive_data)
void free_stack(stack sc);

#endif
