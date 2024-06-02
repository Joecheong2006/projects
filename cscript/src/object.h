#ifndef OBJECT_H
#define OBJECT_H
#include "parser.h"
#include "basic/string.h"

typedef enum {
    ObjectVariable,
    ObjectFunction,
} ObjectType;

typedef struct object object;
struct object {
    void* info;
    string name;
    ObjectType type;
};

typedef struct {
    void* value;
    i32 size;
    i32 type; // Keyword TODO: change to NodeType
} variable_info;

variable_info* make_variable_info(tree_node* node);
void free_object_variable(variable_info* obj);

typedef struct {
    vector(string) params;
    vector(tree_node*) body;
} function_info;

function_info* make_function_info(tree_node* node);
void free_object_function(function_info* obj);

object* make_object(object* obj);
void free_object(void* data);

#endif
