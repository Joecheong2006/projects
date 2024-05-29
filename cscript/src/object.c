#include "object.h"

#include "basic/memallocate.h"
#include <string.h>

object* make_object(object* obj) {
    object* result = MALLOC(sizeof(object));
    memcpy(result, obj, sizeof(object));
    return result;
}

object_variable* make_object_variable(tree_node* node) {
    object_variable* result = MALLOC(sizeof(object_variable));
    result->type = node->object_type;
    switch (result->type) {
    case NodeTypeInt: result->value = MALLOC(sizeof(int)); break;
    case NodeTypeFloat: result->value = MALLOC(sizeof(float)); break;
    case NodeTypeChar: result->value = MALLOC(sizeof(char)); break;
    default: break;
    }
    return result;
}

void free_object_variable(object_variable* obj) {
    FREE(obj->value);
    FREE(obj);
}

void free_object(void* data) {
    object* obj = data;
    switch (obj->type) {
    case ObjectVariable: free_object_variable(obj->info); break;
    default: break;
    }
    free_string(&obj->name);
    FREE(obj);
}

