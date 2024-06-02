#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"

typedef struct {
    u64 index;
    vector(tree_node*) instructions;
} interpreter;

typedef struct {
    i32 type;
    temp_data val;
} data_chunk;

void type_cast(data_chunk* chunk, i32 type);
void interpret(tree_node* ins);

#endif
