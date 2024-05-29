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
} object_variable;

typedef struct {
    void* info;
    string name;
    ObjectType type;
} object;

object* make_object(object* obj);
object_variable* make_object_variable(tree_node* node);
void free_object_variable(object_variable* obj);
void free_object(void* data);

#endif
