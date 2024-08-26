#include "primitive_data.h"
#include "error_info.h"
#include "tracing.h"

INLINE i32 primitive_data_guess_type(primitive_data* a, primitive_data* b) {
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
    default:\
        return (error_info){ .msg = error_msg };\
    }

error_info primitive_data_cast_to(i32 type, primitive_data* pd) {
    START_PROFILING();
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
    default:
        return (error_info){ .msg = "invalid type to cast" };
    }
    pd->type[2] = type;
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

#define IMPL_PRIMITIVE_ARITHMETIC(oper, name)\
error_info primitive_data_##name(primitive_data* out, primitive_data* a, primitive_data* b) {\
    START_PROFILING();\
    i32 type = primitive_data_guess_type(a, b);\
    error_info ei = primitive_data_cast_to(type, a);\
    if (ei.msg)\
        return ei;\
    ei = primitive_data_cast_to(type, b);\
    if (ei.msg)\
        return ei;\
    out->type[2] = type;\
    switch (type) {\
    case PrimitiveDataTypeInt32:\
        out->int32 = a->int32 oper b->int32;\
        break;\
    case PrimitiveDataTypeFloat32:\
        out->float32 = a->float32 oper b->float32;\
        break;\
    case PrimitiveDataTypeInt64:\
        out->int64 = a->int64 oper b->int64;\
        break;\
    case PrimitiveDataTypeFloat64:\
        out->float64 = a->float64 oper b->float64;\
        break;\
    default:\
        return (error_info){ .msg = "unkown primitive_data" };\
    }\
    END_PROFILING(__func__);\
    return (error_info){ .msg = NULL };\
}

IMPL_PRIMITIVE_ARITHMETIC(+, add)
IMPL_PRIMITIVE_ARITHMETIC(-, minus)
IMPL_PRIMITIVE_ARITHMETIC(*, multiply)
IMPL_PRIMITIVE_ARITHMETIC(/, divide)

error_info primitive_data_modulus(primitive_data* out, primitive_data* a, primitive_data* b) {
    START_PROFILING();
    i32 type = primitive_data_guess_type(a, b);
    error_info ei = primitive_data_cast_to(type, a);
    if (ei.msg)
        return ei;
    ei = primitive_data_cast_to(type, b);
    if (ei.msg)
        return ei;
    out->type[2] = type;
    switch (type) {
    case PrimitiveDataTypeInt32:
        out->int32 = a->int32 % b->int32;
        break;
    case PrimitiveDataTypeInt64:
        out->int64 = a->int64 % b->int64;
        break;
    case PrimitiveDataTypeFloat32:
        return (error_info){ .msg = "Invalid modulus operation for float32" };
    case PrimitiveDataTypeFloat64:
        return (error_info){ .msg = "Invalid modulus operation for float64" };
    default:
        return (error_info){ .msg = "undefine primitive data type" };
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

error_info primitive_data_negate(primitive_data* out, primitive_data* a) {
    START_PROFILING();
    out->type[2] = a->type[2];
    switch (a->type[2]) {
    case PrimitiveDataTypeInt32:
        out->int32 = -a->int32;
        break;
    case PrimitiveDataTypeInt64:
        out->int64 = -a->int64;
        break;
    case PrimitiveDataTypeFloat32:
        out->float32 = -a->float32;
        break;
    case PrimitiveDataTypeFloat64:
        out->float64 = -a->float64;
        break;
    default:
        return (error_info){ .msg = "unkown primitive_data" };
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

#define IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(oper, assign_name)\
    error_info primitive_data_##assign_name##_assign(primitive_data* a, primitive_data* b) {\
        START_PROFILING();\
        error_info ei = primitive_data_cast_to(a->type[2], b);\
        if (ei.msg)\
            return ei;\
        switch (a->type[2]) {\
        case PrimitiveDataTypeInt32:\
            a->int32 oper b->int32;\
            break;\
        case PrimitiveDataTypeInt64:\
            a->int64 oper b->int64;\
            break;\
        case PrimitiveDataTypeFloat32:\
            a->float32 oper b->float32;\
            break;\
        case PrimitiveDataTypeFloat64:\
            a->float64 oper b->float64;\
            break;\
        default: return (error_info){ .msg = "undefine primitive data type" };\
        }\
        END_PROFILING(__func__);\
        return (error_info){ .msg = NULL };\
    }

IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(+=, add)
IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(-=, minus)
IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(*=, multiply)
IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(/=, divide)

error_info primitive_data_modulus_assign(primitive_data* a, primitive_data* b) {
    START_PROFILING();
    error_info ei = primitive_data_cast_to(a->type[2], b);
    if (ei.msg)
        return ei;
    switch (a->type[2]) {
    case PrimitiveDataTypeInt32:
        a->int32 %= b->int32;
        break;
    case PrimitiveDataTypeInt64:
        a->int64 %= b->int64;
        break;
    case PrimitiveDataTypeFloat32:
        return (error_info){ .msg = "invalid modulus operation for float32" };
    case PrimitiveDataTypeFloat64:
        return (error_info){ .msg = "invalid modulus operation for float64" };
    default: return (error_info){ .msg = "undefine primitive data type" };
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

