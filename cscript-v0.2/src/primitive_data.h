#ifndef _PRIMITIVE_DATA_H_
#define _PRIMITIVE_DATA_H_
#include "core/defines.h"
#include "error_info.h"

typedef enum {
    PrimitiveDataTypeBoolean,
    PrimitiveDataTypeUInt8,
    PrimitiveDataTypeInt8,
    PrimitiveDataTypeUInt16,
    PrimitiveDataTypeInt16,
    PrimitiveDataTypeUInt32,
    PrimitiveDataTypeInt32,
    PrimitiveDataTypeUInt64,
    PrimitiveDataTypeInt64,
    PrimitiveDataTypeFloat32, //NOTE: float32 + int64 -> float32
    PrimitiveDataTypeFloat64,
    PrimitiveDataTypeString,
    PrimitiveDataTypeObjPtr,
} PrimitiveDataType;

extern u8 primitive_size_map[];
extern char* primitive_type_name[];

struct object_carrier;
typedef struct {
    union {
        u8 boolean;
        u8 uint8;
        i8 int8;
        u16 uint16;
        i16 int16;
        u32 uint32;
        i32 int32;
        u64 uint64;
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

error_info primitive_data_neg(primitive_data* out, primitive_data* a);
error_info primitive_data_add(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_sub(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_mul(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_div(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_mod(primitive_data* out, primitive_data* a, primitive_data* b);

error_info primitive_data_add_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_sub_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_mul_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_div_assign(primitive_data* a, primitive_data* b);
error_info primitive_data_mod_assign(primitive_data* a, primitive_data* b);

error_info primitive_data_cmp_equal(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_not_equal(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_greater_than(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_less_than(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_greater_than_equal(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_cmp_less_than_equal(primitive_data* out, primitive_data* a, primitive_data* b);

error_info primitive_data_and(primitive_data* out, primitive_data* a, primitive_data* b);
error_info primitive_data_or(primitive_data* out, primitive_data* a, primitive_data* b);

#endif
