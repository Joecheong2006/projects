#include "primitive_data.h"
#include "core/log.h"

u8 primitive_size_map[] = {
    [PrimitiveDataTypeBoolean] = 1,
    [PrimitiveDataTypeInt32] = 4,
    [PrimitiveDataTypeInt64] = 8,
    [PrimitiveDataTypeFloat32] = 4,
    [PrimitiveDataTypeFloat64] = 8,
    [PrimitiveDataTypeString] = sizeof(void*),
    [PrimitiveDataTypeObjPtr] = sizeof(void*),
};

void print_primitive_data(primitive_data* data) {
    if (data->type == PrimitiveDataTypeInt64) {
        LOG_DEBUG_MSG("%lld\n", data->val.int64);
    }
    else if (data->type == PrimitiveDataTypeFloat64) {
        LOG_DEBUG_MSG("%lg\n", data->val.float64);
    }
    else if (data->type == PrimitiveDataTypeInt32) {
        LOG_DEBUG_MSG("%d\n", data->val.int32);
    }
    else if (data->type == PrimitiveDataTypeFloat32) {
        LOG_DEBUG_MSG("%g\n", data->val.float32);
    }
}

INLINE i32 primitive_data_guess_type(primitive_data* a, primitive_data* b) {
    return a->type > b->type ? a->type : b->type;
}

#define IMPL_PRIMITIVE_DATA_CAST(cast_type, error_msg)\
    switch (pd->type) {\
    case PrimitiveDataTypeInt32:\
        pd->val.cast_type = pd->val.int32;\
        break;\
    case PrimitiveDataTypeInt64:\
        pd->val.cast_type = pd->val.int64;\
        break;\
    case PrimitiveDataTypeFloat32:\
        pd->val.cast_type = pd->val.float32;\
        break;\
    case PrimitiveDataTypeFloat64:\
        pd->val.cast_type = pd->val.float64;\
        break;\
    case PrimitiveDataTypeBoolean:\
        pd->val.cast_type = pd->val.boolean;\
        break;\
    default:\
        return (error_info){ .msg = error_msg };\
    }

error_info primitive_data_cast_to(i32 type, primitive_data* pd) {
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
    pd->type = type;
    return (error_info){ .msg = NULL };
}

#define IMPL_PRIMITIVE_ARITHMETIC(oper, name)\
error_info primitive_data_##name(primitive_data* out, primitive_data* a, primitive_data* b) {\
    if (a->type == PrimitiveDataTypeBoolean || b->type == PrimitiveDataTypeBoolean) {\
        return (error_info){ .msg = "attempt to perform arithmetic operation on a boolean value" };\
    }\
    i32 type = primitive_data_guess_type(a, b);\
    error_info ei = primitive_data_cast_to(type, a);\
    if (ei.msg)\
        return ei;\
    ei = primitive_data_cast_to(type, b);\
    if (ei.msg)\
        return ei;\
    out->type = type;\
    switch (type) {\
    case PrimitiveDataTypeInt32:\
        out->val.int32 = a->val.int32 oper b->val.int32;\
        break;\
    case PrimitiveDataTypeFloat32:\
        out->val.float32 = a->val.float32 oper b->val.float32;\
        break;\
    case PrimitiveDataTypeInt64:\
        out->val.int64 = a->val.int64 oper b->val.int64;\
        break;\
    case PrimitiveDataTypeFloat64:\
        out->val.float64 = a->val.float64 oper b->val.float64;\
        break;\
    default:\
        return (error_info){ .msg = "unkown primitive_data" };\
    }\
    return (error_info){ .msg = NULL };\
}

IMPL_PRIMITIVE_ARITHMETIC(+, add)
IMPL_PRIMITIVE_ARITHMETIC(-, sub)
IMPL_PRIMITIVE_ARITHMETIC(*, mul)

error_info primitive_data_div(primitive_data* out, primitive_data* a, primitive_data* b) {
    i32 type = primitive_data_guess_type(a, b);
    error_info ei = primitive_data_cast_to(type, a);
    if (ei.msg)
        return ei;
    ei = primitive_data_cast_to(type, b);
    if (ei.msg)
        return ei;
    out->type = type;
    switch (type) {
    case PrimitiveDataTypeInt32: {
        if (b->val.int32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->val.int32 = a->val.int32 / b->val.int32;
        break;
    }
    case PrimitiveDataTypeFloat32: {
        if (b->val.float32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->val.float32 = a->val.float32 / b->val.float32;
        break;
    }
    case PrimitiveDataTypeInt64: {
        if (b->val.int64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->val.int64 = a->val.int64 / b->val.int64;
        break;
    }
    case PrimitiveDataTypeFloat64: {
        if (b->val.float64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        out->val.float64 = a->val.float64 / b->val.float64;
        break;
    }
    default:
        return (error_info){ .msg = "unkown primitive_data" };
    }
    return (error_info){ .msg = NULL };
}

error_info primitive_data_mod(primitive_data* out, primitive_data* a, primitive_data* b) {
    i32 type = primitive_data_guess_type(a, b);
    error_info ei = primitive_data_cast_to(type, a);
    if (ei.msg)
        return ei;
    ei = primitive_data_cast_to(type, b);
    if (ei.msg)
        return ei;
    out->type = type;
    switch (type) {
    case PrimitiveDataTypeInt32:
        if (b->val.int32 == 0) { return (error_info){ .msg = "attemp to perform n % 0" }; }
        out->val.int32 = a->val.int32 % b->val.int32;
        break;
    case PrimitiveDataTypeInt64:
        if (b->val.int64 == 0) { return (error_info){ .msg = "attemp to perform n % 0" }; }
        out->val.int64 = a->val.int64 % b->val.int64;
        break;
    case PrimitiveDataTypeFloat32:
        return (error_info){ .msg = "Invalid modulus operation for float32" };
    case PrimitiveDataTypeFloat64:
        return (error_info){ .msg = "Invalid modulus operation for float64" };
    default:
        return (error_info){ .msg = "undefine primitive data type" };
    }
    return (error_info){ .msg = NULL };
}

error_info primitive_data_neg(primitive_data* out, primitive_data* a) {
    out->type = a->type;
    switch (a->type) {
    case PrimitiveDataTypeInt32:
        out->val.int32 = -a->val.int32;
        break;
    case PrimitiveDataTypeInt64:
        out->val.int64 = -a->val.int64;
        break;
    case PrimitiveDataTypeFloat32:
        out->val.float32 = -a->val.float32;
        break;
    case PrimitiveDataTypeFloat64:
        out->val.float64 = -a->val.float64;
        break;
    default:
        return (error_info){ .msg = "unkown primitive_data" };
    }
    return (error_info){ .msg = NULL };
}

#define IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(oper, assign_name)\
    error_info primitive_data_##assign_name##_assign(primitive_data* a, primitive_data* b) {\
        error_info ei = primitive_data_cast_to(a->type, b);\
        if (ei.msg)\
            return ei;\
        switch (a->type) {\
        case PrimitiveDataTypeInt32:\
            a->val.int32 oper b->val.int32;\
            break;\
        case PrimitiveDataTypeInt64:\
            a->val.int64 oper b->val.int64;\
            break;\
        case PrimitiveDataTypeFloat32:\
            a->val.float32 oper b->val.float32;\
            break;\
        case PrimitiveDataTypeFloat64:\
            a->val.float64 oper b->val.float64;\
            break;\
        default: return (error_info){ .msg = "undefine primitive data type" };\
        }\
        return (error_info){ .msg = NULL };\
    }

IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(+=, add)
IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(-=, sub)
IMPL_PRIMITIVE_ASSIGN_ARITHMETIC(*=, mul)

error_info primitive_data_div_assign(primitive_data* a, primitive_data* b) {
    error_info ei = primitive_data_cast_to(a->type, b);
    if (ei.msg)
        return ei;
    switch (a->type) {
    case PrimitiveDataTypeInt32: {
        if (b->val.int32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->val.int32 /= b->val.int32;
        break;
    }
    case PrimitiveDataTypeInt64: {
        if (b->val.int64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->val.int64 /= b->val.int64;
        break;
    }
    case PrimitiveDataTypeFloat32: {
        if (b->val.float32 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->val.float32 /= b->val.float32;
        break;
    }
    case PrimitiveDataTypeFloat64: {
        if (b->val.float64 == 0) return (error_info){ .msg = "Division by zero is undefined" };
        a->val.float64 /= b->val.float64;
        break;
    }
    default: return (error_info){ .msg = "undefine primitive data type" };
    }
    return (error_info){ .msg = NULL };
}

error_info primitive_data_mod_assign(primitive_data* a, primitive_data* b) {
    error_info ei = primitive_data_cast_to(a->type, b);
    if (ei.msg)
        return ei;
    switch (a->type) {
    case PrimitiveDataTypeInt32:
        a->val.int32 %= b->val.int32;
        break;
    case PrimitiveDataTypeInt64:
        a->val.int64 %= b->val.int64;
        break;
    case PrimitiveDataTypeFloat32:
        return (error_info){ .msg = "invalid modulus operation for float32" };
    case PrimitiveDataTypeFloat64:
        return (error_info){ .msg = "invalid modulus operation for float64" };
    default: return (error_info){ .msg = "undefine primitive data type" };
    }
    return (error_info){ .msg = NULL };
}

#define IMPL_PRIMITIVE_CMP(cmp_oper, name)\
error_info primitive_data_cmp_##name(primitive_data* out, primitive_data* a, primitive_data* b) {\
    if (a->type == PrimitiveDataTypeBoolean || b->type == PrimitiveDataTypeBoolean) {\
        return (error_info){ .msg = "attempt to compare boolean to number" };\
    }\
    i32 type = primitive_data_guess_type(a, b);\
    error_info ei = primitive_data_cast_to(type, a);\
    if (ei.msg)\
        return ei;\
    ei = primitive_data_cast_to(type, b);\
    if (ei.msg)\
        return ei;\
    out->type = PrimitiveDataTypeBoolean;\
    switch (type) {\
    case PrimitiveDataTypeInt32:\
        out->val.boolean = a->val.int32 cmp_oper b->val.int32;\
        break;\
    case PrimitiveDataTypeInt64:\
        out->val.boolean = a->val.int64 cmp_oper b->val.int64;\
        break;\
    case PrimitiveDataTypeFloat32:\
        out->val.boolean = a->val.float32 cmp_oper b->val.float32;\
        break;\
    case PrimitiveDataTypeFloat64:\
        out->val.boolean = a->val.float64 cmp_oper b->val.float64;\
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

