#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"
#include "object.h"

typedef struct {
    u64 index;
    vector(vector(tree_node*)) instructions;
} interpretor;

typedef struct {
    i32 type;
    union {
        char _char;
        int _int;
        float _float;
    } val;
} data_chunk;

void interpret(interpretor* inter);
void interpret_cal_expression_char(char* out, tree_node* node);
void interpret_cal_expression_int(int* out, tree_node* node);
void interpret_cal_expression_float(float* out, tree_node* node);
void interpret_cal_expression(void* out, NodeType data_type, tree_node* node);
void interpret_cal_data_chunk_expression(data_chunk* out, tree_node* node);
void interpret_assignment_operation(object_variable* var, tree_node* assign_expr);
void type_cast(data_chunk* chunk, i32 type);
void evaluate_expression_type(i32* out, tree_node* expression);

#define DEFINE_DATA_CHUNK_CONVERSION(conversion_name, operator) void conversion_name##_data_chunk(void* out, data_chunk chunk);

DEFINE_DATA_CHUNK_CONVERSION(assign, =)
DEFINE_DATA_CHUNK_CONVERSION(plus_equal, +=)
DEFINE_DATA_CHUNK_CONVERSION(minus_equal, -=)
DEFINE_DATA_CHUNK_CONVERSION(multiply_equal, *=)
DEFINE_DATA_CHUNK_CONVERSION(division_equal, /=)

#endif
