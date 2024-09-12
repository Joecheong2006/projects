#include "object.h"
#include "core/memory.h"
#include "core/assert.h"

object* make_object(ObjectType type, u64 type_size, void(*destroy)(object*)) {
    object* result = cnew_mem(1, type_size + sizeof(object));
    result->type = type;
    result->destroy = destroy;
    return result;
}

INLINE void* get_object_true_type(object* obj) { return obj + 1; }

void object_bool_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeBool);
    free_mem(obj);
}

INLINE object* make_object_bool(void) {
    return make_object(ObjectTypeBool, sizeof(object_bool), object_bool_destroy);
}

void object_primitive_data_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypePrimitiveData);
    free_mem(obj);
}

INLINE object* make_object_primitive_data(void) {
    return make_object(ObjectTypePrimitiveData, sizeof(object_primitive_data), object_primitive_data_destroy);
}

void object_string_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeString);
    object_string* str = get_object_true_type(obj);
    free_string(str->val);
    free_mem(obj);
}

object* make_object_string(void) {
    object* result = make_object(ObjectTypeString, sizeof(object_string), object_string_destroy);
    object_string* string = get_object_true_type(result);
    string->val = make_string("");
    return result;
}

void object_function_def_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeFunctionDef);
    object_function_def* def = get_object_true_type(obj);
    free_vector(def->args);
    free_mem(obj);
}

object* make_object_function_def(void) {
    object* result = make_object(ObjectTypeFunctionDef, sizeof(object_function_def), object_function_def_destroy);
    object_function_def* def = get_object_true_type(result);
    def->args = make_vector(cstring);
    return result;
}

void object_ref_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeRef);
    object_ref* ref = get_object_true_type(obj);
    if (--ref->ref_obj->ref_count == 0) {
        ref->ref_obj->destroy(ref->ref_obj);
    }
    free_mem(obj);
}

object* make_object_ref(void) {
    return make_object(ObjectTypeRef, sizeof(object_ref), object_ref_destroy);
}

void object_user_type_destroy(object* obj) {
    ASSERT(obj->type == ObjectTypeUserType);
    object_user_type* user_type = get_object_true_type(obj);
    for_vector(user_type->members, i, 0) {
        user_type->members[i]->destroy(user_type->members[i]);
    }
    free_vector(user_type->members);
    free_mem(obj);
}

object* make_object_user_type(void) {
    object* result = make_object(ObjectTypeUserType, sizeof(object_user_type), object_user_type_destroy);
    object_user_type* user_type = get_object_true_type(result);
    user_type->members = make_vector(object*);
    return result;

}

