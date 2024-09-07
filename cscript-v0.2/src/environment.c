#include "environment.h"
#include "core/assert.h"
#include "tracing.h"
#include "core/memory.h"
#include <string.h>

INLINE static void scopes_push_obj(environment* env, object_carrier* obj) {
    vector_push(vector_back(env->global), obj);
}

static void free_global_scopes(environment* env) {
    for_vector(env->global, i, 0) {
        free_scope(env, env->global[i]);
        env->global[i] = NULL;
    }
    free_vector(env->global);
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
    object_carrier* carrier = (object_carrier*)data;
    return hash_string(carrier->obj->name) % size;
}

void env_push_scope(environment* env) {
    scope sc = make_scope();
    vector_push(env->global, sc);
}

void env_pop_scope(environment* env) {
    free_scope(env, vector_back(env->global));
    vector_pop(env->global);
}

object_carrier* env_find_object(environment* env, cstring name) {
    vector(void*) result = env->map.data[hash_string(name) % env->map.size];
    for (i64 i = (i64)vector_size(result) - 1; i > -1; --i) {
        object_carrier* carrier = result[i];
        if (strcmp(carrier->obj->name, name) == 0) {
            return carrier;
        }
    }
    return NULL;
}

void env_push_object(environment* env, object_carrier* carrier) {
    carrier->obj->level = vector_size(env->global);
    hashmap_add(&env->map, carrier);
    scopes_push_obj(env, carrier);
}

void env_remove_object_from_scope(environment* env, object_carrier* carrier) {
    vector(void*) result = hashmap_access_vector(&env->map, carrier);
    for (i64 i = 0; i < vector_size(result); ++i) {
        if (result[i] == carrier) {
            for (i64 j = i; j < vector_size(result) - 1; ++j) {
                result[j] = result[j + 1];
            }
            vector_pop(result);
            break;
        }
    }

    scope s = vector_back(env->global);
    for (i64 i = 0; i < vector_size(s); ++i) {
        if (s[i] == carrier) {
            for (i64 j = i; j < vector_size(s) - 1; ++j) {
                s[j] = s[j + 1];
            }
            vector_pop(s);
            return;
        }
    }
}

void env_pop_object(environment* env, object_carrier* carrier) {
    vector(void*) result = hashmap_access_vector(&env->map, carrier);
    carrier->obj->destroy(carrier->obj, env);
    free_mem(carrier);
    vector_pop(result);
}

void init_environment(environment* env) {
    START_PROFILING();
    env->map = make_hashmap(1 << 9, hash_object);
    env->global = make_scopes();
    env->bp = make_stack();
    END_PROFILING(__func__);
}

INLINE i32 get_env_level(environment* env) {
    return vector_size(env->global);
}

void free_environment(environment* env) {
    START_PROFILING();
    free_global_scopes(env);
    free_hashmap(&env->map);
    free_vector(env->bp);
    END_PROFILING(__func__);
}

