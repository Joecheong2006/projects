#include "object.h"

#include "basic/memallocate.h"
#include "parser.h"
#include <string.h>

object* make_object(object* obj) {
    object* result = MALLOC(sizeof(object));
    ASSERT_MSG(result != NULL, "malloc failed");
    memcpy(result, obj, sizeof(object));
    return result;
}

variable_info* make_variable_info(tree_node* node) {
    variable_info* result = MALLOC(sizeof(variable_info));
    ASSERT_MSG(result != NULL, "malloc failed");
    result->type = node->object_type;
    switch (result->type) {
    case NodeTypeInt: result->value = MALLOC(sizeof(i64)); break;
    case NodeTypeFloat: result->value = MALLOC(sizeof(f64)); break;
    case NodeTypeChar: result->value = MALLOC(sizeof(u8));  break;
    case NodeTypeString: result->value = MALLOC(sizeof(char*));  break;
    default: break;
    }
    return result;
}

void free_object_variable(variable_info* obj) {
    if (obj->type == NodeTypeString) {
        FREE(*((char**)obj->value));
    }
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

