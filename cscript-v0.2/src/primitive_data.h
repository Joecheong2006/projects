#ifndef _PRIMITIVE_DATA_H_
#define _PRIMITIVE_DATA_H_
#include "core/defines.h"
#include "error_info.h"

typedef enum {
    PrimitiveDataTypeBoolean,
    PrimitiveDataTypeInt32,
    PrimitiveDataTypeInt64,
    PrimitiveDataTypeFloat32, //NOTE: float32 + int64 -> float32
    PrimitiveDataTypeFloat64,
    PrimitiveDataTypeString,
    PrimitiveDataTypeObjPtr,
} PrimitiveDataType;

struct object_carrier;
typedef struct {
    union {
        u8 boolean;
        i32 int32;
        i64 int64;
        f32 float32;
        f64 float64;
        const char* string;
        struct object_carrier* carrier;
    } val;
    i32 type;
} primitive_data;

void print_primitive_data(primitive_data* data);

i32 primitive_data_guess_type(primitive_data* a, primitive_data* b);
error_info primitive_data_cast_to(i32 type, primitive_data* pd);

error_info primitive_data_negate(primitive_data* out, primitive_data* a);
error_info primitive_data_add(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_minus(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_multiply(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_divide(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_modulus(primitive_data* out, primitive_data* a, primitive_data* b);

error_info primitive_data_add_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_minus_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_multiply_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_divide_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_modulus_assign(primitive_data* a, primitive_data* b);

error_info primitive_data_cmp_equal(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_not_equal(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_greater_than(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_less_than(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_greater_than_equal(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_less_than_equal(primitive_data* out, primitive_data* a, primitive_data* b);

#endif
