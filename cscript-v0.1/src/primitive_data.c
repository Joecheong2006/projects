#include "primitive_data.h"
#include "lexer.h"

i32 primitive_data_guess_type(primitive_data* a, primitive_data* b) {
    return a->type[2] > b->type[2] ? a->type[2] : b->type[2];
}

void primitive_data_cast_to(i32 type, primitive_data* pd) {
    switch (type - pd->type[2]) {
    case TokenTypeLiteralInt32 - TokenTypeLiteralFloat32: {
        pd->int32 = pd->float32;
        pd->type[2] = type;
        break;
    }
    case TokenTypeLiteralFloat32 - TokenTypeLiteralInt32: {
        pd->float32 = pd->int32;
        pd->type[2] = type;
        break;
    }
    default:
        break;
    }
}

#define IMPL_PRIMITIVE_ARITHMETIC(oper, name)\
primitive_data primitive_data_##name(primitive_data* a, primitive_data* b) {\
    i32 type = primitive_data_guess_type(a, b);\
    primitive_data result = {.type[2] = type};\
    primitive_data_cast_to(type, a);\
    primitive_data_cast_to(type, b);\
    switch (type) {\
    case TokenTypeLiteralInt32: {\
        result.int32 = a->int32 oper b->int32;\
        return result;\
    }\
    case TokenTypeLiteralFloat32: {\
        result.float32 = a->float32 oper b->float32;\
        return result;\
    }\
    default:\
        return result;\
    }\
}

IMPL_PRIMITIVE_ARITHMETIC(+, add)
IMPL_PRIMITIVE_ARITHMETIC(-, minus)
IMPL_PRIMITIVE_ARITHMETIC(*, multiply)
IMPL_PRIMITIVE_ARITHMETIC(/, divide)

primitive_data primitive_data_negate(primitive_data* a) {
    primitive_data result = {.type[2] = a->type[2]};
    switch (a->type[2]) {
    case TokenTypeLiteralInt32: {
        result.int32 = -a->int32;
        return result;
    }
    case TokenTypeLiteralFloat32: {
        result.float32 = -a->float32;
        return result;
    }
    default:
        return result;
    }
}
