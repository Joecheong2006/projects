#include "object.h"
#include "container/memallocate.h"
#include "command.h"
#include "core/assert.h"

object* make_object(ObjectType type, cstring name, u64 type_size, void(*destroy)(object*)) {
    object* result = MALLOC(type_size + sizeof(object));
    result->type = type;
    result->name = name;
    result->destroy = destroy;
    return result;
}

INLINE void* get_object_true_type(object* obj) { return obj + 1; }

void object_bool_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeBool);
    FREE(obj);
}

object* make_object_bool(cstring name) {
    return make_object(ObjectTypeBool, name, sizeof(object_bool), object_bool_destroy);
}

void object_primitive_data_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypePrimitiveData);
    FREE(obj);
}

object* make_object_primitive_data(cstring name) {
    return make_object(ObjectTypePrimitiveData, name, sizeof(object_primitive_data), object_primitive_data_destroy);
}

void object_string_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeString);
    object_string* str = get_object_true_type(obj);
    free_string(str->val);
    FREE(obj);
}

object* make_object_string(cstring name) {
    object* result = make_object(ObjectTypeString, name, sizeof(object_string), object_string_destroy);
    object_string* string = get_object_true_type(result);
    string->val = make_string("");
    return result;
}

void object_function_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeFunction);
    object_function* func = get_object_true_type(obj);
    for_vector(func->body, i, 0) {
        func->body[i]->destroy(func->body[i]);
    }
    for_vector(func->args, i, 0) {
        free_string(func->args[i]);
    }
    free_vector(func->body);
    free_vector(func->args);
    FREE(obj);
}

object* make_object_function(cstring name) {
    object* result = make_object(ObjectTypeFunction, name, sizeof(object_function), object_function_destroy);
    object_function* func = get_object_true_type(result);
    func->body = make_vector(command*);
    func->args = make_vector(cstring);
    return result;
}

void object_user_type_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeUserType);
    object_user_type* user_type = get_object_true_type(obj);
    for_vector(user_type->members, i, 0) {
        user_type->members[i]->destroy(user_type->members[i]);
    }
    free_vector(user_type->members);
    FREE(obj);
}

object* make_object_user_type(cstring name) {
    object* result = make_object(ObjectTypeUserType, name, sizeof(object_user_type), object_user_type_destroy);
    object_user_type* user_type = get_object_true_type(result);
    user_type->members = make_vector(object*);
    return result;
}

