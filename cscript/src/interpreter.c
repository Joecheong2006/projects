#include "interpreter.h"
#include "keys_define.h"
#include "environment.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INTERPRET_OPERATE_NUMBERS(type, data, operator)\
        type lhs_value = get_node_number_value_##type(lhs);\
        type rhs_value = get_node_number_value_##type(rhs);\
        type sum = lhs_value operator rhs_value + *(type*)out;\
        memcpy(data, &sum, sizeof(type));

#define INPERPRET_OPERATOR_ARITHMETIC(data_type, type, out, lhs, rhs)\
    switch (data_type) {\
    case OperatorPlus: {\
        type result = *(type*)lhs + *(type*)rhs;\
        memcpy(out, &result, sizeof(type));\
    } break;\
    case OperatorMinus: {\
        type result = *(type*)lhs - *(type*)rhs;\
        memcpy(out, &result, sizeof(type));\
    } break;\
    case OperatorMultiply: {\
        type result = *(type*)lhs * *(type*)rhs;\
        memcpy(out, &result, sizeof(type));\
    } break;\
    case OperatorDivision: {\
        type result = *(type*)lhs / *(type*)rhs;\
        memcpy(out, &result, sizeof(type));\
    } break;\
    default: break;\
    }

#define CPY_NEGATE(type, out)\
    interpret_cal_expression_##type(out, node->nodes[0]);\
    *(type*)out = -*(type*)out;

#define CPY_DEC_NUMBER(type, node, out)\
    {type value = get_node_number_value_##type(node);\
    memcpy(out, &value, sizeof(type));}

#define CPY_HEX_NUMBER(type, node, out)\
    {type value = hex_to_dec(node);\
    memcpy(out, &value, sizeof(type));}

#define CPY_OCT_NUMBER(type, node, out)\
    {type value = oct_to_dec(node);\
    memcpy(out, &value, sizeof(type));}

#define CPY_BIN_NUMBER(type, node, out)\
    {type value = bin_to_dec(node);\
    memcpy(out, &value, sizeof(type));}

#define CPY_CHAR_LITERAL(type, node, out)\
    {type value = node->name[0];\
    memcpy(out, &value, sizeof(type));}

#define TYPE_CONVERSION(type_a, type_b, value)\
    {type_a val = *(type_a*)value;\
    type_b _val = val;\
    *out = _val;}

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

void interpret_cal_expression_int(int* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        int lhs = 0, rhs = 0;
        interpret_cal_expression_int(&lhs, node->nodes[0]);
        interpret_cal_expression_int(&rhs, node->nodes[1]);
        INPERPRET_OPERATOR_ARITHMETIC(node->object_type, int, out, &lhs, &rhs);
    } break;
    case NodeNegateOperator: CPY_NEGATE(int, out); break;
    case NodeDecNumber: CPY_DEC_NUMBER(int, node, out); break;
    case NodeHexNumber: CPY_HEX_NUMBER(int, node, out); break;
    case NodeOctNumber: CPY_OCT_NUMBER(int, node, out); break;
    case NodeBinNumber: CPY_BIN_NUMBER(int, node, out); break;
    case NodeCharLiteral: CPY_CHAR_LITERAL(int, node, out); break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (!obj) {
            // NOTE: report run time error
        }
        object_variable* var = obj->info;
        switch (var->type) {
            case NodeTypeInt: *out = *(int*)var->value; break;
            case NodeTypeChar: *out = *(char*)var->value; break;
            default: break;
        }
        break;
    } break;
    default: break;
    }
}

void interpret_cal_expression_float(float* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        float lhs = 0, rhs = 0;
        interpret_cal_expression_float(&lhs, node->nodes[0]);
        interpret_cal_expression_float(&rhs, node->nodes[1]);
        INPERPRET_OPERATOR_ARITHMETIC(node->object_type, float, out, &lhs, &rhs);
    } break;
    case NodeNegateOperator: CPY_NEGATE(float, out); break;
    case NodeDecNumber: CPY_DEC_NUMBER(float, node, out); break;
    case NodeHexNumber: CPY_HEX_NUMBER(float, node, out); break;
    case NodeOctNumber: CPY_OCT_NUMBER(float, node, out); break;
    case NodeBinNumber: CPY_BIN_NUMBER(float, node, out); break;
    case NodeCharLiteral: CPY_CHAR_LITERAL(float, node, out); break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (!obj) {
            // NOTE: report run time error
        }
        object_variable* var = obj->info;
        switch (var->type) {
            case NodeTypeInt: TYPE_CONVERSION(int, float, var->value); break;
            case NodeTypeChar: TYPE_CONVERSION(char, float, var->value); break;
            case NodeTypeFloat: *out = *(float*)var->value; break;
            default: break;
        }
        break;
    } break;
    default: break;
    }
}

void interpret_cal_expression_char(char* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        int lhs = 0, rhs = 0;
        interpret_cal_expression_int(&lhs, node->nodes[0]);
        interpret_cal_expression_int(&rhs, node->nodes[1]);
        INPERPRET_OPERATOR_ARITHMETIC(node->object_type, char, out, &lhs, &rhs);
    } break;
    case NodeDecNumber: {
        char value = get_node_number_value_int(node);
        memcpy(out, &value, sizeof(char));
    } break;
    case NodeHexNumber: CPY_HEX_NUMBER(char, node, out); break;
    case NodeOctNumber: CPY_OCT_NUMBER(char, node, out); break;
    case NodeBinNumber: CPY_BIN_NUMBER(char, node, out); break;
    case NodeCharLiteral: CPY_CHAR_LITERAL(char, node, out); break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (!obj) {
            // NOTE: report run time error
        }
        object_variable* var = obj->info;
        switch (var->type) {
        case NodeTypeFloat: TYPE_CONVERSION(float, char, var->value); break;
        case NodeTypeInt: *out = *(int*)var->value; break;
        case NodeTypeChar: *out = *(char*)var->value; break;
        default: break;
        }
        break;
    } break;
    default: break;
    }
}

void interpret_cal_expression(void* out, NodeType data_type, tree_node* node) {
    switch (data_type) {
    case NodeTypeChar: interpret_cal_expression_char(out, node); break;
    case NodeTypeInt: interpret_cal_expression_int(out, node); break;
    case NodeTypeFloat: interpret_cal_expression_float(out, node); break;
    default: break;
    }
}

void interpret_cal_data_chunk_expression(data_chunk* out, tree_node* node) {
    switch (out->type) {
    case NodeTypeChar: interpret_cal_expression_char(&out->val._char, node); break;
    case NodeTypeInt: interpret_cal_expression_int(&out->val._int, node); break;
    case NodeTypeFloat: interpret_cal_expression_float(&out->val._float, node); break;
    default: break;
    }
}

void evaluate_expression_type(i32* out, tree_node* expression) {
    for_vector(expression->nodes, i, 0) {
        evaluate_expression_type(out, expression->nodes[i]);
    }
    if (expression->type >= NodeTypeInt && expression->type <= NodeTypeChar && expression->object_type > *out) {
        *out = expression->object_type;
    }
}

void interpret_assignment_operation(object_variable* var, tree_node* assign_expr) {
    i32 expr_type = -1;
    evaluate_expression_type(&expr_type, assign_expr->nodes[0]);
    data_chunk chunk = { .type = expr_type };
    interpret_cal_data_chunk_expression(&chunk, assign_expr->nodes[0]);
    type_cast(&chunk, var->type);

    switch (assign_expr->object_type) {
    case OperatorPlusEqual: plus_equal_data_chunk(var->value, chunk); break;
    case OperatorMinusEqual: minus_equal_data_chunk(var->value, chunk); break;
    case OperatorMultiplyEqual: multiply_equal_data_chunk(var->value, chunk); break;
    case OperatorDivisionEqual: division_equal_data_chunk(var->value, chunk); break;
    default: break;
    }
}

void type_cast(data_chunk* chunk, i32 type) {
    switch (type - chunk->type) {
        case NodeTypeInt - NodeTypeFloat: chunk->val._int = (int)chunk->val._float; break;
        case NodeTypeFloat - NodeTypeInt: chunk->val._float = (float)chunk->val._int; break;
        default: break;
    }
    chunk->type = type;
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
IMPL_DATA_CHUNK_CONVERSION(plus_equal, +=)
IMPL_DATA_CHUNK_CONVERSION(minus_equal, -=)
IMPL_DATA_CHUNK_CONVERSION(multiply_equal, *=)
IMPL_DATA_CHUNK_CONVERSION(division_equal, /=)

