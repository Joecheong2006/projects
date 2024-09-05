#include "object.h"
#include "container/memallocate.h"
#include "core/assert.h"
#include "tracing.h"
#include "core/log.h"
#include "environment.h"
#include "debug_flags.h"

#if defined(OBJECT_DEBUG_LOG) && defined(DEBUG)
#define LOG LOG_DEBUG
#else
#define LOG(...)
#endif

object* make_object(ObjectType type, cstring name, u64 type_size, void(*destroy)(object*, struct environment* inter)) {
    object* result = CALLOC(1, type_size + sizeof(object));
    result->type = type;
    result->name = name;
    result->destroy = destroy;
    return result;
}

INLINE void* get_object_true_type(object* obj) { return obj + 1; }

void object_none_destroy(object* obj, struct environment* inter) {
    (void)inter;
    LOG("\t\tdestory none '%s'\n", obj->name);
    ASSERT(obj->type == ObjectTypeNone);
    FREE(obj);
}

INLINE object* make_object_none(cstring name) {
    LOG("\t\tmake none '%s'\n", name);
    return make_object(ObjectTypeNone, name, 0, object_none_destroy);
}

void object_bool_destroy(object* obj, struct environment* inter) {
    (void)inter;
    LOG("\t\tdestory bool '%s'\n", obj->name);
    ASSERT(obj->type == ObjectTypeBool);
    FREE(obj);
}

INLINE object* make_object_bool(cstring name) {
    LOG("\t\tmake bool '%s'\n", name);
    return make_object(ObjectTypeBool, name, sizeof(object_bool), object_bool_destroy);
}

void object_primitive_data_destroy(object* obj, struct environment* inter) {
    (void)inter;
    LOG("\t\tdestroy primitive_data '%s'\n", obj->name);
    ASSERT(obj->type == ObjectTypePrimitiveData);
    FREE(obj);
}

INLINE object* make_object_primitive_data(cstring name) {
    LOG("\t\tmake primitive_data '%s'\n", name);
    return make_object(ObjectTypePrimitiveData, name, sizeof(object_primitive_data), object_primitive_data_destroy);
}

void object_string_destroy(object* obj, struct environment* inter) {
    (void)inter;
    LOG("\t\tdestroy string '%s'\n", obj->name);
    ASSERT(obj->type == ObjectTypeString);
    object_string* str = get_object_true_type(obj);
    free_string(str->val);
    FREE(obj);
}

object* make_object_string(cstring name) {
    LOG("\t\tmake string '%s'\n", name);
    START_PROFILING();
    object* result = make_object(ObjectTypeString, name, sizeof(object_string), object_string_destroy);
    object_string* string = get_object_true_type(result);
    string->val = make_string("");
    END_PROFILING(__func__);
    return result;
}

void object_function_def_destroy(object* obj, struct environment* inter) {
    LOG("\t\tdestroy funcdef '%s'\n", obj->name);
    (void)inter;
    ASSERT(obj->type == ObjectTypeFunctionDef);
    FREE(obj);
}

object* make_object_function_def(cstring name) {
    LOG("\t\tmake funcdef '%s'\n", name);
    START_PROFILING();
    object* result = make_object(ObjectTypeFunctionDef, name, sizeof(object_function_def), object_function_def_destroy);
    END_PROFILING(__func__);
    return result;
}

void object_ref_destroy(object* obj, struct environment* inter) {
    LOG("\t\tdestroy ref '%s'\n", obj->name);
    ASSERT(obj->type == ObjectTypeRef);
    object_ref* ref = get_object_true_type(obj);
    object_carrier* ref_obj = env_find_object(inter, ref->ref_name);
    ref_obj->obj->ref_count--;
    LOG("\t\t'%s' drop %s' ref to %d\n", obj->name, ref_obj->obj->name, ref_obj->obj->ref_count);
    FREE(obj);
}

object* make_object_ref(cstring name) {
    LOG("\t\tmake ref %s'\n", name);
    START_PROFILING();
    object* result = make_object(ObjectTypeRef, name, sizeof(object_ref), object_ref_destroy);
    END_PROFILING(__func__);
    return result;
}

void object_user_type_destroy(object* obj, struct environment* inter) {
    LOG("\t\tmake type %s'\n", obj->name);
    ASSERT(obj->type == ObjectTypeUserType);
    object_user_type* user_type = get_object_true_type(obj);
    for_vector(user_type->members, i, 0) {
        user_type->members[i]->destroy(user_type->members[i], inter);
    }
    free_vector(user_type->members);
    FREE(obj);
}

object* make_object_user_type(cstring name) {
    LOG("\t\tmake user_type %s'\n", name);
    START_PROFILING();
    object* result = make_object(ObjectTypeUserType, name, sizeof(object_user_type), object_user_type_destroy);
    object_user_type* user_type = get_object_true_type(result);
    user_type->members = make_vector(object*);
    END_PROFILING(__func__);
    return result;

}

