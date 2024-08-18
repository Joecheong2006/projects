#include "object.h"
#include "container/memallocate.h"
#include "command.h"
#include <assert.h>

object* make_object(ObjectType type, const char* name, u64 type_size, void(*destroy)(object*)) {
    object* result = MALLOC(type_size + sizeof(object));
    result->type = type;
    result->name = name;
    result->destroy = destroy;
    return result;
}

void free_object(object* obj) {
    obj->destroy(obj);
    free_string(obj->name);
    FREE(obj);
}

void* get_object_true_type(object* obj) { return obj + sizeof(object); }

void object_int_destroy(object* obj) {
    assert(obj->type == ObjectTypeInt);
}

object* make_object_int(const char* name) {
    return make_object(ObjectTypeInt, name, sizeof(object_int), object_int_destroy);
}

void object_float_destroy(object* obj) {
    assert(obj->type == ObjectTypeFloat);
}

object* make_object_float(const char* name) {
    return make_object(ObjectTypeInt, name, sizeof(object_float), object_int_destroy);
}

void object_string_destroy(object* obj) {
    assert(obj->type == ObjectTypeString);
}

object* make_object_string(const char* name) {
    return make_object(ObjectTypeInt, name, sizeof(object_string), object_int_destroy);
}

void object_function_destroy(object* obj) {
    assert(obj->type == ObjectTypeFunction);
    object_function* func = get_object_true_type(obj);
    for_vector(func->body, i, 0) {
        free_command(func->body[i]);
    }
    for_vector(func->args, i, 0) {
        free_string(func->args[i]);
    }
    free_vector(func->body);
    free_vector(func->args);
}

object* make_object_function(const char* name) {
    return make_object(ObjectTypeFunction, name, sizeof(object_function), object_function_destroy);
}

void object_user_type_destroy(object* obj) {
    assert(obj->type == ObjectTypeUserType);
    object_user_type* user_type = get_object_true_type(obj);
    for_vector(user_type->members, i, 0) {
        free_object(user_type->members + i);
    }
    free_vector(user_type->members);
}

object* make_object_user_type(const char* name) {
    return make_object(ObjectTypeUserType, name, sizeof(object_user_type), object_user_type_destroy);
}
