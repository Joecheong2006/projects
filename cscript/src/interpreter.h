#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"

typedef struct {
    u64 index;
    vector(tree_node*) instructions;
} interpretor;

typedef struct {
    i32 type;
    union {
        char _char;
        int _int;
        float _float;
    } val;
} data_chunk;

void type_cast(data_chunk* chunk, i32 type);
void interpret(tree_node* ins);

#define DEFINE_DATA_CHUNK_CONVERSION(conversion_name, operator) void conversion_name##_data_chunk(void* out, data_chunk chunk);

#endif
