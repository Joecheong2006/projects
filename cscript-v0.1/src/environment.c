#include "environment.h"
#include "core/assert.h"
#include "tracing.h"
#include <string.h>

INLINE static void scopes_push_obj(scopes s, object* obj) {
    vector_push(vector_back(s), obj);
}

INLINE static void scopes_pop_obj(scopes s) {
    vector_pop(vector_back(s));
}

static void free_scope(scope sc) {
    for (i64 i = (i64)vector_size(sc) - 1; i > -1; --i) {
        sc[i]->destroy(sc[i]);
    }
    free_vector(sc);
}

static void free_global_scopes(scopes s) {
    for_vector(s, i, 0) {
        free_scope(s[i]);
        s[i] = NULL;
    }
    free_vector(s);
}

static u32 hash_string(const char* str) {
    u32 hash = 2166136261u;
    u32 length = strlen(str);
    for (u32 i = 0; i < length; i++) {
        hash ^= (u8)str[i];
        hash *= 16777619;
    }
    return hash;
}

INLINE static u32 hash_object(void* data, u32 size) {
    ASSERT(data);
    object* obj = data;
    return hash_string(obj->name) % size;
    // return sdbm(obj->name) % size;
}

void env_push_scope(environment* env) {
    scope sc = make_scope();
    vector_push(env->global, sc);
}

void env_pop_scope(environment* env) {
    scope sc = vector_back(env->global);
    free_scope(sc);
    vector_pop(env->global);
}

object* env_find_object(environment* env, cstring name) {
    START_PROFILING();
    object obj = { .name = name };
    vector(void*) result = hashmap_access_vector(&env->map, &obj);
    for (i64 i = (i64)vector_size(result) - 1; i > -1; --i) {
        object* obj = result[i];
        if (strcmp(obj->name, name) == 0) {
            return obj;
        }
    }
    END_PROFILING(__func__);
    return NULL;
}

void env_push_object(environment* env, object* obj) {
    START_PROFILING();
    scopes_push_obj(env->global, obj);
    hashmap_add(&env->map, obj);
    END_PROFILING(__func__);
}

void env_pop_object(environment* env) {
    START_PROFILING();
    object tmp = { .name = vector_back(vector_back(env->global))->name };
    vector(void*) result = hashmap_access_vector(&env->map, &tmp);
    object* obj = vector_back(result);
    obj->destroy(obj);
    vector_pop(result);
    // for (i64 i = vector_size(result) - 1; i > -1; --i) {
    //     object* obj = result[i];
    //     if (strcmp(obj->name, tmp.name) == 0) {
    //         obj->destroy(obj);
    //         for (i64 j = i; j < vector_size(result) - 1; --j) {
    //             result[j] = result[j + 1];
    //         }
    //         vector_pop(result);
    //         break;
    //     }
    // }
    scopes_pop_obj(env->global);
    END_PROFILING(__func__);
}

void init_environment(environment* env) {
    START_PROFILING();
    env->map = make_hashmap(1 << 9, hash_object);
    env->global = make_scopes();
    END_PROFILING(__func__);
}

void free_environment(environment* env) {
    START_PROFILING();
    free_hashmap(&env->map);
    free_global_scopes(env->global);
    END_PROFILING(__func__);
}

