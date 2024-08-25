#include "primitive_data.h"
#include "object.h"

i32 primitive_type_map[] = {
    ObjectTypeInt32,
    ObjectTypeInt64,
    ObjectTypeFloat32,
    ObjectTypeFloat64,
};

i32 primitive_data_guess_type(primitive_data* a, primitive_data* b) {
    return a->type[2] > b->type[2] ? a->type[2] : b->type[2];
}

#define IMPL_PRIMITIVE_DATA_CAST(cast_type, error_msg)\
    switch (pd->type[2]) {\
    case PrimitiveDataTypeInt32:\
        pd->cast_type = pd->int32;\
        break;\
    case PrimitiveDataTypeInt64:\
        pd->cast_type = pd->int64;\
        break;\
    case PrimitiveDataTypeFloat32:\
        pd->cast_type = pd->float32;\
        break;\
    case PrimitiveDataTypeFloat64:\
        pd->cast_type = pd->float64;\
        break;\
    default: {\
        pd->type[2] = -1;\
        pd->string = error_msg;\
        return;\
    }\
    }

void primitive_data_cast_to(i32 type, primitive_data* pd) {
    switch (type) {
    case PrimitiveDataTypeInt32: {
        IMPL_PRIMITIVE_DATA_CAST(int32, "invalid cast to int32");
        break;
    }
    case PrimitiveDataTypeInt64: {
        IMPL_PRIMITIVE_DATA_CAST(int64, "invalid cast to int64");
        break;
    }
    case PrimitiveDataTypeFloat32: {
        IMPL_PRIMITIVE_DATA_CAST(float32, "invalid cast to float32");
        break;
    }
    case PrimitiveDataTypeFloat64: {
        IMPL_PRIMITIVE_DATA_CAST(float64, "invalid cast to float64");
        break;
    }
    default: {
        pd->type[2] = -1;
        pd->string = "invalid type to cast";
        return;
    }
    }
    pd->type[2] = type;
}

#define IMPL_PRIMITIVE_ARITHMETIC(oper, name)\
primitive_data primitive_data_##name(primitive_data* a, primitive_data* b) {\
    i32 type = primitive_data_guess_type(a, b);\
    primitive_data result = {.type[2] = type};\
    primitive_data_cast_to(type, a);\
    if (a->type[2] < 0) return *a;\
    primitive_data_cast_to(type, b);\
    if (b->type[2] < 0) return *b;\
    switch (type) {\
    case PrimitiveDataTypeInt32: {\
        result.int32 = a->int32 oper b->int32;\
        return result;\
    }\
    case PrimitiveDataTypeFloat32: {\
        result.float32 = a->float32 oper b->float32;\
        return result;\
    }\
    default: {\
        result.type[2] = -1;\
        result.string = "unkown primitive_data";\
        return result;\
    }\
    }\
}

IMPL_PRIMITIVE_ARITHMETIC(+, add)
IMPL_PRIMITIVE_ARITHMETIC(-, minus)
IMPL_PRIMITIVE_ARITHMETIC(*, multiply)
IMPL_PRIMITIVE_ARITHMETIC(/, divide)

primitive_data primitive_data_modulus(primitive_data* a, primitive_data* b) {
    i32 type = primitive_data_guess_type(a, b);
    primitive_data result = {.type[2] = type};
    primitive_data_cast_to(type, a);
    if (a->type[2] < 0) return *a;
    primitive_data_cast_to(type, b);
    if (b->type[2] < 0) return *b;
    switch (type) {
    case PrimitiveDataTypeInt32: {
        result.int32 = a->int32 % b->int32;
        return result;
    }
    default: {
        result.type[2] = -1;
        result.string = "Invalid operands to binary expression";
        return result;
    }
    }
}

primitive_data primitive_data_negate(primitive_data* a) {
    primitive_data result = {.type[2] = a->type[2]};
    switch (a->type[2]) {
    case PrimitiveDataTypeInt32: {
        result.int32 = -a->int32;
        return result;
    }
    case PrimitiveDataTypeFloat32: {
        result.float32 = -a->float32;
        return result;
    }
    default: {
        result.type[2] = -1;
        result.string = "unkown primitive_data";
        return result;
    }
    }
}

