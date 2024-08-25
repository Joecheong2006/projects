#ifndef _PRIMITIVE_DATA_H_
#define _PRIMITIVE_DATA_H_
#include "container/util.h"

typedef enum {
    PrimitiveDataTypeInt32,
    PrimitiveDataTypeInt64,
    PrimitiveDataTypeFloat32,
    PrimitiveDataTypeFloat64,
} PrimitiveDataType;

extern i32 primitive_type_map[];

typedef union {
    i32 int32;
    f32 float32;
    i64 int64;
    f64 float64;
    const char* string;
    i32 type[3];
} primitive_data;

struct error_info;
i32 primitive_data_guess_type(primitive_data* a, primitive_data* b);
struct error_info primitive_data_cast_to(i32 type, primitive_data* pd);
struct error_info primitive_data_add(primitive_data* out, primitive_data* a, primitive_data* b);
struct error_info primitive_data_minus(primitive_data* out, primitive_data* a, primitive_data* b);
struct error_info primitive_data_multiply(primitive_data* out, primitive_data* a, primitive_data* b);
struct error_info primitive_data_divide(primitive_data* out, primitive_data* a, primitive_data* b);
struct error_info primitive_data_modulus(primitive_data* out, primitive_data* a, primitive_data* b);
struct error_info primitive_data_negate(primitive_data* out, primitive_data* a);
struct error_info primitive_data_add_assign(primitive_data* a, primitive_data* b);
struct error_info primitive_data_minus_assign(primitive_data* a, primitive_data* b);
struct error_info primitive_data_multiply_assign(primitive_data* a, primitive_data* b);
struct error_info primitive_data_divide_assign(primitive_data* a, primitive_data* b);
struct error_info primitive_data_modulus_assign(primitive_data* a, primitive_data* b);

#endif
