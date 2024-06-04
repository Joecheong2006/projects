#include "object.h"

#include "environment.h"
#include "basic/memallocate.h"
#include "parser.h"
#include <string.h>

variable_info* make_variable_info(void* data) {
    variable_info* result = MALLOC(sizeof(variable_info));
    ASSERT_MSG(result != NULL, "malloc failed");
    result->type = *(i32*)data;
    switch (result->type) {
    case NodeTypeInt: { result->size = sizeof(i64); } break;
    case NodeTypeFloat: { result->size = sizeof(f64); } break;
    case NodeTypeChar: { result->size = sizeof(u8); }  break;
    case NodeTypeString: { result->val._string = NULL, result->size = sizeof(char*); }  break;
    default: break;
    }
    return result;
}

function_info* make_function_info(tree_node* node) {
    function_info* result = MALLOC(sizeof(function_info));
    ASSERT_MSG(result != NULL, "malloc failed");
    result->params = make_vector();
    for_vector(node->nodes, i, 0) {
        tree_node* param = node->nodes[i];
        vector_push(result->params, make_stringn(param->name, param->name_len));
    }
    result->body = make_vector();
    do {
        vector_push(result->body, env.inter.instructions[++env.inter.index]);
    } while (env.inter.instructions[env.inter.index]->type != NodeEnd);

    return result;
}

void free_object_variable(variable_info* obj) {
    if (obj->type == NodeTypeString && obj->val._string != NULL) {
        FREE(obj->val._string);
    }
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

object* make_ref_object(object* obj) {
    ASSERT_MSG(obj != NULL, "invalid object");
    object* result = make_object(&(object){
            .name = NULL,
            .type = obj->type,
            .info = obj->info,
            });
    result->ref_object = obj;
    ++result->ref_object->ref_count;
    return result;
}

static variable_info* copy_variable_info(variable_info* info) {
    variable_info * result = make_variable_info(&info->type);
    memcpy(&result->val, &info->val, sizeof(temp_data));
    return result;
}

object* copy_object(object* obj) {
    ASSERT_MSG(obj != NULL, "invalid object");
    object* result = make_object(&(object){ .name = NULL, .type = obj->type, .info = NULL, });
    switch (obj->type) {
    case ObjectVariable: result->info = copy_variable_info(obj->info); break;
    default: {
        FREE(result);
        return NULL; 
    }
    }
    return result;
}

void register_object(object* obj) {
    vector_push(vector_back(env.scopes), obj);
    hashmap_add(env.object_map, obj);
}

object* make_object(object* obj) {
    object* result = MALLOC(sizeof(object));
    ASSERT_MSG(result != NULL, "malloc failed");
    memcpy(result, obj, sizeof(object));
    result->ref_count = 0;
    result->ref_object = NULL;
    return result;
}

void free_object(void* data) {
    object* obj = data;
    if (!obj->ref_object) {
        switch (obj->type) {
            case ObjectVariable: free_object_variable(obj->info); break;
            case ObjectFunction: free_object_function(obj->info); break;
            default: break;
        }
    }
    free_string(&obj->name);
    FREE(obj);
}

