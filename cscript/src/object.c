#include "object.h"

#include "basic/memallocate.h"
#include <string.h>

object* make_object(object* obj) {
    object* result = MALLOC(sizeof(object));
    memcpy(result, obj, sizeof(object));
    return result;
}

object_variable* make_object_variable(object_variable* obj) {
    object_variable* result = MALLOC(sizeof(object_variable));
    memcpy(result, obj, sizeof(object_variable));
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

