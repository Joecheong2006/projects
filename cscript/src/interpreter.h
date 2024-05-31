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
        u8 _char;
        char* _string;
        i64 _int;
        f64 _float;
    } val;
} data_chunk;

void type_cast(data_chunk* chunk, i32 type);
void interpret(tree_node* ins);

#endif
