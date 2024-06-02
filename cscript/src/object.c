#include "object.h"

#include "environment.h"
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
    case NodeTypeInt: { result->value = MALLOC(sizeof(i64)); result->size = sizeof(i64); } break;
    case NodeTypeFloat: { result->value = MALLOC(sizeof(f64)); result->size = sizeof(f64); } break;
    case NodeTypeChar: { result->value = MALLOC(sizeof(u8)); result->size = sizeof(u8); }  break;
    case NodeTypeString: { result->value = MALLOC(sizeof(char*)); result->size = sizeof(char*); }  break;
    default: break;
    }
    return result;
}

function_info* make_function_info(tree_node* node) {
    function_info* result = MALLOC(sizeof(function_info));
    ASSERT_MSG(result != NULL, "malloc failed");
    result->params = make_vector();
    for_vector(node->nodes[0]->nodes, i, 0) {
        tree_node* param = node->nodes[0]->nodes[i];
        vector_push(result->params, make_stringn(param->name, param->name_len));
    }
    result->body = make_vector();
    while (env.inter.instructions[++env.inter.index]->type != NodeEnd) {
        vector_push(result->body, env.inter.instructions[env.inter.index]);
    }
    return result;
}

void free_object_variable(variable_info* obj) {
    if (obj->value == NULL) {
        FREE(obj);
        return;
    }
    if (obj->type == NodeTypeString) {
        FREE(*((char**)obj->value));
    }
    FREE(obj->value);
    FREE(obj);
}

void free_object_function(function_info* obj) {
    for_vector(obj->params, i, 0) {
        free_string(&obj->params[i]);
    }
    free_vector(&obj->params);
    free_vector(&obj->body);
    FREE(obj);
}

void free_object(void* data) {
    object* obj = data;
    switch (obj->type) {
        case ObjectVariable: free_object_variable(obj->info); break;
        case ObjectFunction: free_object_function(obj->info); break;
        default: break;
    }
    free_string(&obj->name);
    FREE(obj);
}

