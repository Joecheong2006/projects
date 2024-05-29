#include "interpreter.h"
#include "keys_define.h"
#include "environment.h"
#include "object.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define INTERPRET_OPERATE_NUMBERS(type, data, operator)\
        type lhs_value = get_node_number_value_##type(lhs);\
        type rhs_value = get_node_number_value_##type(rhs);\
        type sum = lhs_value operator rhs_value + *(type*)out;\
        memcpy(data, &sum, sizeof(type));

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

static i32 hex_to_dec_ch(const char c) {
    return c >= 'a' && c <= 'f' ? c - 'W' : c - '0';
}

static i32 hex_to_dec(tree_node* tok) {
    return base_n_to_dec(tok, 16, hex_to_dec_ch);
}

static i32 bin_to_dec_ch(const char c) {
    return c == '1';
}

static i32 bin_to_dec(tree_node* tok) {
    return base_n_to_dec(tok, 2, bin_to_dec_ch);
}

static i32 oct_to_dec_ch(const char c) {
    return c - '0';
}

static i32 oct_to_dec(tree_node* tok) {
    return base_n_to_dec(tok, 8, oct_to_dec_ch);
}


static INLINE int get_node_number_value_int(tree_node* node) {
    return atoi(node->name);
}

static INLINE float get_node_number_value_float(tree_node* node) {
    return atof(node->name);
}

static INLINE void interpret_operator_plus_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs + *rhs;
    memcpy(out, &sum, sizeof(int));
}

static INLINE void interpret_operator_minus_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs - *rhs;
    memcpy(out, &sum, sizeof(int));
}

static INLINE void interpret_operator_multiply_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs * *rhs;
    memcpy(out, &sum, sizeof(int));
}

static INLINE void interpret_operator_division_int(int* out, int* lhs, int* rhs) {
    int sum = *lhs / *rhs;
    memcpy(out, &sum, sizeof(int));
}

static INLINE void interpret_operator_plus_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs + *rhs;
    memcpy(out, &sum, sizeof(float));
}

static INLINE void interpret_operator_minus_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs - *rhs;
    memcpy(out, &sum, sizeof(float));
}

static INLINE void interpret_operator_multiply_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs * *rhs;
    memcpy(out, &sum, sizeof(float));
}

static INLINE void interpret_operator_division_float(float* out, float* lhs, float* rhs) {
    float sum = *lhs / *rhs;
    memcpy(out, &sum, sizeof(float));
}

static void interpret_operator_arithmetic_int(OperatorType type, int* out, int* lhs, int* rhs) {
    switch (type) {
    case OperatorPlus: interpret_operator_plus_int(out, lhs, rhs); break;
    case OperatorMinus: interpret_operator_minus_int(out, lhs, rhs); break;
    case OperatorMultiply: interpret_operator_multiply_int(out, lhs, rhs); break;
    case OperatorDivision: interpret_operator_division_int(out, lhs, rhs); break;
    default: break;
    }
}

static void interpret_operator_arithmetic_float(OperatorType type, float* out, float* lhs, float* rhs) {
    switch (type) {
    case OperatorPlus: interpret_operator_plus_float(out, lhs, rhs); break;
    case OperatorMinus: interpret_operator_minus_float(out, lhs, rhs); break;
    case OperatorMultiply: interpret_operator_multiply_float(out, lhs, rhs); break;
    case OperatorDivision: interpret_operator_division_float(out, lhs, rhs); break;
    default: break;
    }
}

static void interpret_cal_expression_int(int* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        int lhs = 0, rhs = 0;
        interpret_cal_expression_int(&lhs, node->nodes[0]);
        interpret_cal_expression_int(&rhs, node->nodes[1]);
        interpret_operator_arithmetic_int(node->object_type, out, &lhs, &rhs);
    } break;
    case NodeNegateOperator: {
        interpret_cal_expression_int(out, node->nodes[0]);
        *out = -*out;
    } break;
    case NodeDecNumber: {
        int value = get_node_number_value_int(node);
        memcpy(out, &value, sizeof(int));
    } break;
    case NodeHexNumber: {
        int value = hex_to_dec(node);
        memcpy(out, &value, sizeof(int));
    } break;
    case NodeOctNumber: {
        int value = oct_to_dec(node);
        memcpy(out, &value, sizeof(int));
    } break;
    case NodeBinNumber: {
        int value = bin_to_dec(node);
        memcpy(out, &value, sizeof(int));
    } break;
    case NodeCharLiteral: {
        int value = node->name[0];
        memcpy(out, &value, sizeof(int));
    } break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (obj) {
            object_variable* var = obj->info;
            switch (var->type) {
            case NodeTypeInt: {
                *out = *(int*)var->value;
            } break;
            case NodeTypeChar: {
                *out = *(char*)var->value;
            } break;
            default: break;
            }
            break;
        }
        // NOTE: run time error
    } break;
    default: break;
    }
}

static void interpret_cal_expression_float(float* out, tree_node* node) {
    switch (node->type) {
    case NodeOperator: {
        float lhs = 0, rhs = 0;
        interpret_cal_expression_float(&lhs, node->nodes[0]);
        interpret_cal_expression_float(&rhs, node->nodes[1]);
        interpret_operator_arithmetic_float(node->object_type, out, &lhs, &rhs);
    } break;
    case NodeNegateOperator: {
        interpret_cal_expression_float(out, node->nodes[0]);
        *out = -*out;
    } break;
    case NodeDecNumber: {
        float value = get_node_number_value_float(node);
        memcpy(out, &value, sizeof(float));
    }break;
    case NodeHexNumber: {
        float value = hex_to_dec(node);
        memcpy(out, &value, sizeof(float));
    } break;
    case NodeOctNumber: {
        float value = oct_to_dec(node);
        memcpy(out, &value, sizeof(float));
    } break;
    case NodeBinNumber: {
        float value = bin_to_dec(node);
        memcpy(out, &value, sizeof(float));
    } break;
    case NodeCharLiteral: {
        float value = node->name[0];
        memcpy(out, &value, sizeof(float));
    } break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (obj) {
            object_variable* var = obj->info;
            switch (var->type) {
            case NodeTypeInt: {
                int val = *(int*)var->value;
                float float_val = val;
                *out = float_val;
            } break;
            case NodeTypeFloat: {
                *out = *(float*)var->value;
            } break;
            case NodeTypeChar: {
                *out = *(char*)var->value;
            } break;
            default: break;
            }
            break;
        }
        // NOTE: run time error
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
        int int_out = 0;
        interpret_operator_arithmetic_int(node->object_type, &int_out, &lhs, &rhs);
        *out = int_out;
    } break;
    case NodeDecNumber: {
        char value = get_node_number_value_int(node);
        memcpy(out, &value, sizeof(char));
    } break;
    case NodeHexNumber: {
        char value = hex_to_dec(node);
        memcpy(out, &value, sizeof(char));
    } break;
    case NodeOctNumber: {
        char value = oct_to_dec(node);
        memcpy(out, &value, sizeof(char));
    } break;
    case NodeBinNumber: {
        char value = bin_to_dec(node);
        memcpy(out, &value, sizeof(char));
    } break;
    case NodeCharLiteral: {
        *out = node->name[0];
    } break;
    case NodeVariable: {
        object* obj = get_object(node->name, node->name_len);
        if (obj) {
            object_variable* var = obj->info;
            switch (var->type) {
            case NodeTypeFloat: {
                char c = *(float*)var->value;
                *out = c;
            } break;
            case NodeTypeInt: {
                *out = *(int*)var->value;
            } break;
            case NodeTypeChar: {
                *out = *(char*)var->value;
            } break;
            default: break;
            }
            break;
        }
        // NOTE: run time error
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

