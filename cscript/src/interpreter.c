#include "interpreter.h"
#include "keys_define.h"
#include "environment.h"
#include "object.h"

#include <stdlib.h>
#include <math.h>

static int base_n_to_dec(tree_node* node, int base_n, i32(*is_digit)(const char c)) {
    int result = 0;
    const char* str = node->name + 2;
    int len = node->name_len - 2;
    for (; str[0] == '0'; str++, len--) {}

    for (i32 i = 0; i < len; ++i) {
        result += powl(base_n, i) * is_digit(str[len - i - 1]);
    }
    return result;
}

static INLINE i32 hex_to_dec_ch(const char c) {
    return c >= 'a' && c <= 'f' ? c - 'W' : c - '0';
}

static INLINE i32 hex_to_dec(tree_node* tok) {
    return base_n_to_dec(tok, 16, hex_to_dec_ch);
}

static INLINE i32 bin_to_dec_ch(const char c) {
    return c == '1';
}

static INLINE i32 bin_to_dec(tree_node* tok) {
    return base_n_to_dec(tok, 2, bin_to_dec_ch);
}

static INLINE i32 oct_to_dec_ch(const char c) {
    return c - '0';
}

static INLINE i32 oct_to_dec(tree_node* tok) {
    return base_n_to_dec(tok, 8, oct_to_dec_ch);
}

static INLINE int get_node_number_value_int(tree_node* node) {
    return atoi(node->name);
}

static INLINE float get_node_number_value_float(tree_node* node) {
    return atof(node->name);
}

#define IMPL_DATA_CHUNK_CONVERSION(conversion_name, operator)\
    void conversion_name##_data_chunk(void* out, data_chunk chunk) {\
        switch (chunk.type) {\
        case NodeTypeChar: *(char*)out operator chunk.val._char; break;\
        case NodeTypeInt: *(int*)out operator chunk.val._int; break;\
        case NodeTypeFloat: *(float*)out operator chunk.val._float; break;\
        default: break;\
        }\
    }

IMPL_DATA_CHUNK_CONVERSION(assign, =)
IMPL_DATA_CHUNK_CONVERSION(negate, = -)
IMPL_DATA_CHUNK_CONVERSION(plus_equal, +=)
IMPL_DATA_CHUNK_CONVERSION(minus_equal, -=)
IMPL_DATA_CHUNK_CONVERSION(multiply_equal, *=)
IMPL_DATA_CHUNK_CONVERSION(division_equal, /=)

#define IMPL_DATA_CHUNK_ARITHMETIC(name, operation)\
    void name##_data_chunk(data_chunk* out, data_chunk* a, data_chunk* b) {\
        out->type = a->type > b->type ? a->type : b->type;\
        type_cast(a, out->type);\
        type_cast(b, out->type);\
        switch (out->type) {\
            case NodeTypeInt: out->val._int = a->val._int operation b->val._int; break;\
            case NodeTypeChar: out->val._char = a->val._char operation b->val._char; break;\
            case NodeTypeFloat:out->val._float = a->val._float operation b->val._float; break;\
            default: break;\
        }\
    }

IMPL_DATA_CHUNK_ARITHMETIC(add, +)
IMPL_DATA_CHUNK_ARITHMETIC(minus, -)
IMPL_DATA_CHUNK_ARITHMETIC(multiply, *)
IMPL_DATA_CHUNK_ARITHMETIC(division, /)

static void interpret_cal_expression(data_chunk* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        data_chunk lhs, rhs;
        interpret_cal_expression(&lhs, node->nodes[0]);
        interpret_cal_expression(&rhs, node->nodes[1]);
        switch (node->object_type) {
        case OperatorPlus: add_data_chunk(out, &lhs, &rhs); break;
        case OperatorMinus: minus_data_chunk(out, &lhs, &rhs); break;
        case OperatorMultiply: multiply_data_chunk(out, &lhs, &rhs); break;
        case OperatorDivision: division_data_chunk(out, &lhs, &rhs); break;
        default: break;
        }
        } break;
    case NodeNegateOperator: {
        interpret_cal_expression(out, node->nodes[0]);
        negate_data_chunk(&out->val, *out);
    } break;
    case NodeDecNumber: {
        out->type = node->object_type;
        switch (node->object_type) {
        case NodeTypeInt: out->val._int = get_node_number_value_int(node); break;
        case NodeTypeFloat: out->val._float = get_node_number_value_float(node); break;
        default: break;
        }
    } break;
    case NodeHexNumber: {
        out->type = node->object_type;
        out->val._int = hex_to_dec(node);
    } break;
    case NodeOctNumber: {
        out->type = node->object_type;
        out->val._int = oct_to_dec(node);
    } break;
    case NodeBinNumber: {
        out->type = node->object_type;
        out->val._int = bin_to_dec(node);
    } break;
    case NodeCharLiteral: {
        out->type = node->object_type;
        out->val._char = node->name[0];
    } break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (!obj) {
            // NOTE: report run time error
        }
        variable_info* var = obj->info;
        out->type = var->type;
        switch (out->type) {
            case NodeTypeInt: out->val._int = *(int*)var->value; break;
            case NodeTypeChar: out->val._char = *(int*)var->value; break;
            case NodeTypeFloat: out->val._float = *(int*)var->value; break;
            default: break;
        }
        break;
    } break;
    default: break;
    }
}

void type_cast(data_chunk* chunk, i32 type) {
    switch (type - chunk->type) {
    case NodeTypeInt - NodeTypeFloat: chunk->val._int = (int)chunk->val._float; break;
    case NodeTypeInt - NodeTypeChar: chunk->val._int = (int)chunk->val._char; break;
    case NodeTypeChar - NodeTypeFloat: chunk->val._char = (char)chunk->val._float; break;
    case NodeTypeChar - NodeTypeInt: chunk->val._char = (char)chunk->val._int; break;
    case NodeTypeFloat - NodeTypeInt: chunk->val._float = (float)chunk->val._int; break;
    case NodeTypeFloat - NodeTypeChar: chunk->val._float = (float)chunk->val._char; break;
    default: break;
    }
    chunk->type = type;
}

static void print_variable(object* obj) {
    variable_info* info = obj->info;
    switch (info->type) {
        case NodeTypeInt: printf("%s = %d\n", obj->name, *(i32*)info->value); break;
        case NodeTypeFloat: printf("%s = %g\n", obj->name, *(f32*)info->value); break;
        case NodeTypeChar: printf("%s = %c\n", obj->name, *(char*)info->value); break;
        default: break;
    }
}

// TEST: temp func
void register_object(object* obj) {
    vector_push(vector_back(env.scopes), obj);
    hashmap_add(env.object_map, obj);
}

static void interpret_variable_initialize(tree_node* node) {
    data_chunk rhs = { .type = -1 };
    interpret_cal_expression(&rhs, node->nodes[0]->nodes[0]);

    node->object_type = rhs.type;
    object* obj = get_object(node->name, node->name_len);

    if (obj) {
        variable_info* var = obj->info;
        if (var->type != node->object_type) {
            free_object_variable(obj->info);
            obj->info = make_variable_info(node);
        }
    }
    else {
        obj = make_object(&(object){
                .name = make_stringn(node->name, node->name_len),
                .type = ObjectVariable,
                .info = make_variable_info(node),
                });

        register_object(obj);
    }

    variable_info* var = obj->info;
    assign_data_chunk(var->value, rhs);
    print_variable(obj);
}

static void interpret_variable_assignment(tree_node* node) {
    object* obj = get_object(node->name, node->name_len);
    if (!obj) {
        printf("not found object ");
        print_token_name(&(token){ .name = node->name, .name_len = node->name_len });
        putchar('\n');
        exit(1);
    }

    variable_info* info = obj->info;
    data_chunk chunk = { .type = -1 };
    interpret_cal_expression(&chunk, node->nodes[0]->nodes[0]);
    type_cast(&chunk, info->type);

    switch (node->nodes[0]->object_type) {
    case OperatorPlusEqual: plus_equal_data_chunk(info->value, chunk); break;
    case OperatorMinusEqual: minus_equal_data_chunk(info->value, chunk); break;
    case OperatorMultiplyEqual: multiply_equal_data_chunk(info->value, chunk); break;
    case OperatorDivisionEqual: division_equal_data_chunk(info->value, chunk); break;
    default: break;
    }

    print_variable(obj);
}

void interpret(tree_node* ins) {
    switch (ins->type) {
    case NodeVariableInitialize: interpret_variable_initialize(ins); break;
    case NodeVariableAssignment: interpret_variable_assignment(ins); break;
    default: printf("not implement node instruction %d yet\n", ins->type);  break;
    }
}

