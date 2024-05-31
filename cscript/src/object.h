#ifndef OBJECT_H
#define OBJECT_H
#include "parser.h"
#include "basic/string.h"

typedef enum {
    ObjectVariable,
    ObjectFunction,
} ObjectType;

typedef struct {
    void* value;
    i32 type; // Keyword TODO: change to NodeType
} variable_info;

typedef struct {
    void* info;
    string name;
    ObjectType type;
} object;

object* make_object(object* obj);
variable_info* make_variable_info(tree_node* node);
void free_object_variable(variable_info* obj);
void free_object(void* data);

#endif
