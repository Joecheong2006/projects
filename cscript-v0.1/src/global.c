#include "global.h"
#include "core/assert.h"
#include "tracing.h"
#include <string.h> // NOTE: for strcmp()

static hashmap map;
static scopes global_scopes;

INLINE static void scopes_push_obj(object* obj) {
    vector_push(vector_back(global_scopes), obj);
}

INLINE static void scopes_pop_obj(void) {
    vector_pop(vector_back(global_scopes));
}

static void free_scope(scope sc) {
    for (i64 i = vector_size(sc) - 1; i > -1; --i) {
        sc[i]->destroy(sc[i]);
    }
    free_vector(sc);
}

static void free_global_scopes(void) {
    for_vector(global_scopes, i, 0) {
        free_scope(global_scopes[i]);
        global_scopes[i] = NULL;
    }
    free_vector(global_scopes);
}

INLINE static u32 hash_object(void* data, u32 size) {
    ASSERT(data);
    object* obj = data;
    return sdbm(obj->name) % size;
}

void setup_global_env(void) {
    START_PROFILING();
    map = make_hashmap(1 << 10, hash_object);
    global_scopes = make_scopes();
    END_PROFILING(__func__);
}

void shutdown_global_env(void) {
    START_PROFILING();
    free_global_scopes();
    free_hashmap(&map);
    END_PROFILING(__func__);
}

void scopes_push(void) {
    ASSERT_MSG(global_scopes, "uninitialize global env");
    scope sc = make_scope();
    vector_push(global_scopes, sc);
}

void scopes_pop(void) {
    ASSERT_MSG(global_scopes, "uninitialize global env");
    scope sc = vector_back(global_scopes);
    free_scope(sc);
    vector_pop(global_scopes);
}

object* find_object(cstring name) {
    ASSERT_MSG(map.data, "uninitialize global env");
    START_PROFILING();
    object obj = { .name = name };
    vector(void*) result = hashmap_access_vector(&map, &obj);
    for (i64 i = (i64)vector_size(result) - 1; i > -1; --i) {
        object* obj = result[i];
        if (strcmp(obj->name, name) == 0) {
            return obj;
        }
    }
    END_PROFILING(__func__);
    return NULL;
}

void pop_object(void) {
    ASSERT_MSG(map.data, "uninitialize global env");
    START_PROFILING();
    object tmp = { .name = vector_back(vector_back(global_scopes))->name };
    vector(void*) result = hashmap_access_vector(&map, &tmp);
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
    scopes_pop_obj();
    END_PROFILING(__func__);
}

void push_object(object* obj) {
    START_PROFILING();
    scopes_push_obj(obj);
    hashmap_add(&map, obj);
    END_PROFILING(__func__);
}

