#ifndef INTERPRETER_H
#define INTERPRETER_H
#include "parser.h"

typedef struct {
    u64 index;
    vector(vector(tree_node*)) instructions;
} interpretor;

void interpret(interpretor* inter);
void interpret_cal_expression(void* out, NodeType data_type, tree_node* node);

#endif
