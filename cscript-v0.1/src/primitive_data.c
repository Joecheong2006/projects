#include "primitive_data.h"
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
    case PrimitiveDataTypeBoolean:\
        pd->cast_type = pd->boolean;\
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
    case PrimitiveDataTypeBoolean: {
        IMPL_PRIMITIVE_DATA_CAST(float64, "invalid cast to boolean");
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
    if (a->type[2] == PrimitiveDataTypeBoolean || b->type[2] == PrimitiveDataTypeBoolean) {\
        return (error_info){ .msg = "attempt to perform arithmetic operation on a boolean value" };\
    }\
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

error_info primitive_data_divide(primitive_data* out, primitive_data* a, primitive_data* b) {
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
    case PrimitiveDataTypeInt32: {
        if (b->int32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->int32 = a->int32 / b->int32;
        break;
    }
    case PrimitiveDataTypeFloat32: {
        if (b->float32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->float32 = a->float32 / b->float32;
        break;
    }
    case PrimitiveDataTypeInt64: {
        if (b->int64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->int64 = a->int64 / b->int64;
        break;
    }
    case PrimitiveDataTypeFloat64: {
        if (b->float64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->float64 = a->float64 / b->float64;
        break;
    }
    default:
        return (error_info){ .msg = "unkown primitive_data" };
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

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

error_info primitive_data_divide_assign(primitive_data* a, primitive_data* b) {
    START_PROFILING();
    error_info ei = primitive_data_cast_to(a->type[2], b);
    if (ei.msg)
        return ei;
    switch (a->type[2]) {
    case PrimitiveDataTypeInt32: {
        if (b->int32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->int32 /= b->int32;
        break;
    }
    case PrimitiveDataTypeInt64: {
        if (b->int64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->int64 /= b->int64;
        break;
    }
    case PrimitiveDataTypeFloat32: {
        if (b->float32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->float32 /= b->float32;
        break;
    }
    case PrimitiveDataTypeFloat64: {
        if (b->float64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->float64 /= b->float64;
        break;
    }
    default: return (error_info){ .msg = "undefine primitive data type" };
    }
    END_PROFILING(__func__);
    return (error_info){ .msg = NULL };
}

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

#define IMPL_PRIMITIVE_CMP(cmp_oper, name)\
error_info primitive_data_cmp_##name(primitive_data* out, primitive_data* a, primitive_data* b) {\
    if (a->type[2] == PrimitiveDataTypeBoolean || b->type[2] == PrimitiveDataTypeBoolean) {\
        return (error_info){ .msg = "attempt to compare boolean to number" };\
    }\
    i32 type = primitive_data_guess_type(a, b);\
    error_info ei = primitive_data_cast_to(type, a);\
    if (ei.msg)\
        return ei;\
    ei = primitive_data_cast_to(type, b);\
    if (ei.msg)\
        return ei;\
    out->type[2] = PrimitiveDataTypeBoolean;\
    switch (type) {\
    case PrimitiveDataTypeInt32:\
        out->boolean = a->int32 cmp_oper b->int32;\
        break;\
    case PrimitiveDataTypeInt64:\
        out->boolean = a->int64 cmp_oper b->int64;\
        break;\
    case PrimitiveDataTypeFloat32:\
        out->boolean = a->float32 cmp_oper b->float32;\
        break;\
    case PrimitiveDataTypeFloat64:\
        out->boolean = a->float64 cmp_oper b->float64;\
        break;\
    default: return (error_info){ .msg = "undefine primitive data type" };\
    }\
    return (error_info){ .msg = NULL };\
}

IMPL_PRIMITIVE_CMP(==, equal)
IMPL_PRIMITIVE_CMP(!=, not_equal)
IMPL_PRIMITIVE_CMP(>, greater_than)
IMPL_PRIMITIVE_CMP(<, less_than)
IMPL_PRIMITIVE_CMP(>=, greater_than_equal)
IMPL_PRIMITIVE_CMP(<=, less_than_equal)

