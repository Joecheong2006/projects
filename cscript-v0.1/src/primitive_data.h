#ifndef _PRIMITIVE_DATA_H_
#define _PRIMITIVE_DATA_H_
#include "container/util.h"

typedef union {
    i32 int32;
    f32 float32;
    const char* string;
    i32 type[3];
} primitive_data;

i32 primitive_data_guess_type(primitive_data* a, primitive_data* b);
void primitive_data_cast_to(i32 type, primitive_data* pd);
primitive_data primitive_data_add(primitive_data* a, primitive_data* b);
primitive_data primitive_data_minus(primitive_data* a, primitive_data* b);
primitive_data primitive_data_multiply(primitive_data* a, primitive_data* b);
primitive_data primitive_data_divide(primitive_data* a, primitive_data* b);
primitive_data primitive_data_negate(primitive_data* a);

#endif
