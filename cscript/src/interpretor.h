#ifndef INTERPRETOR_H
#define INTERPRETOR_H
#include "parser.h"
#include "keys_define.h"

typedef struct {
    u64 index;
    vector(vector(tree_node*)) instructions;
} interpretor;

void interpret(interpretor* inter);
void interpret_cal_expression(void* out, KeywordType data_type, tree_node* node);

#endif
