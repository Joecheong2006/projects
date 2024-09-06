#include "object.h"
#include "core/memory.h"
#include "core/assert.h"
#include "tracing.h"
#include "environment.h"

object* make_object(ObjectType type, cstring name, u64 type_size, void(*destroy)(object*, struct environment* inter)) {
    object* result = cnew_mem(1, type_size + sizeof(object));
    result->type = type;
    result->name = name;
    result->destroy = destroy;
    return result;
}

INLINE void* get_object_true_type(object* obj) { return obj + 1; }

void object_none_destroy(object* obj, struct environment* inter) {
    (void)inter;
    ASSERT(obj->type == ObjectTypeNone);
    free_mem(obj);
}

INLINE object* make_object_none(cstring name) {
    return make_object(ObjectTypeNone, name, 0, object_none_destroy);
}

void object_bool_destroy(object* obj, struct environment* inter) {
    (void)inter;
    ASSERT(obj->type == ObjectTypeBool);
    free_mem(obj);
}

INLINE object* make_object_bool(cstring name) {
    return make_object(ObjectTypeBool, name, sizeof(object_bool), object_bool_destroy);
}

void object_primitive_data_destroy(object* obj, struct environment* inter) {
    (void)inter;
    ASSERT(obj->type == ObjectTypePrimitiveData);
    free_mem(obj);
}

INLINE object* make_object_primitive_data(cstring name) {
    return make_object(ObjectTypePrimitiveData, name, sizeof(object_primitive_data), object_primitive_data_destroy);
}

void object_string_destroy(object* obj, struct environment* inter) {
    (void)inter;
    ASSERT(obj->type == ObjectTypeString);
    object_string* str = get_object_true_type(obj);
    free_string(str->val);
    free_mem(obj);
}

object* make_object_string(cstring name) {
    START_PROFILING();
    object* result = make_object(ObjectTypeString, name, sizeof(object_string), object_string_destroy);
    object_string* string = get_object_true_type(result);
    string->val = make_string("");
    END_PROFILING(__func__);
    return result;
}

void object_function_def_destroy(object* obj, struct environment* inter) {
    (void)inter;
    ASSERT(obj->type == ObjectTypeFunctionDef);
    object_function_def* def = get_object_true_type(obj);
    free_vector(def->args);
    free_mem(obj);
}

object* make_object_function_def(cstring name) {
    START_PROFILING();
    object* result = make_object(ObjectTypeFunctionDef, name, sizeof(object_function_def), object_function_def_destroy);
    object_function_def* def = get_object_true_type(result);
    def->args = make_vector(cstring);
    END_PROFILING(__func__);
    return result;
}

void object_ref_destroy(object* obj, struct environment* inter) {
    ASSERT(obj->type == ObjectTypeRef);
    object_ref* ref = get_object_true_type(obj);
    object_carrier* ref_obj = env_find_object(inter, ref->ref_name);
    ref_obj->obj->ref_count--;
    free_mem(obj);
}

object* make_object_ref(cstring name) {
    START_PROFILING();
    object* result = make_object(ObjectTypeRef, name, sizeof(object_ref), object_ref_destroy);
    END_PROFILING(__func__);
    return result;
}

void object_user_type_destroy(object* obj, struct environment* inter) {
    ASSERT(obj->type == ObjectTypeUserType);
    object_user_type* user_type = get_object_true_type(obj);
    for_vector(user_type->members, i, 0) {
        user_type->members[i]->destroy(user_type->members[i], inter);
    }
    free_vector(user_type->members);
    free_mem(obj);
}

object* make_object_user_type(cstring name) {
    START_PROFILING();
    object* result = make_object(ObjectTypeUserType, name, sizeof(object_user_type), object_user_type_destroy);
    object_user_type* user_type = get_object_true_type(result);
    user_type->members = make_vector(object*);
    END_PROFILING(__func__);
    return result;

}

