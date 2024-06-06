#include "environment.h"
#include <string.h>

struct _environment env;

void add_error_message(error_message message) {
    vector_pushe(env.error_messages, message);
}

u64 hash_object_len(const char* name, i32 len, u64 size) {
    size_t result = 5381;
    for (i32 i = 0; i < len; ++i) {
        result = ((result << 5) + result) + name[i];
    }
    return result % size;
}

object* get_object(const char* name, u64 len) {
    ASSERT_MSG(name != NULL, "invalid name");
    vector(object*) objs = (vector(object*))env.object_map.data[hash_object_len(name, len, env.object_map.size)];
    for (i64 i = vector_size(objs) - 1; i > -1; --i) {
        u64 obj_name = vector_size(objs[i]->name);
        if (obj_name == len && strncmp(objs[i]->name, name, len) == 0) {
            return objs[i];
        }
    }
    return NULL;
}

u64 hash_object(void* data, u64 size) {
    ASSERT_MSG(data != NULL, "hashing invalid data");
    size_t result = 5381;
    char* str = ((object*)data)->name;
    for (i32 i = 0; str[i] != 0; ++i) {
        result = ((result << 5) + result) + str[i];
    }
    return result % size;
}

void init_environment(void) {
    env.scopes = make_vector();
    env.object_map = make_hashmap(1 << 10, hash_object);
    env.error_messages = make_vector();
    env.inter = (interpreter){
        .index = 0,
        .instructions = make_vector()
    };
}

void delete_environment(void) {
    for_vector(env.scopes, i, 0) {
        free_scope(&env.scopes[i]);
    }
    free_vector(&env.scopes);
    free_hashmap(&env.object_map);
    free_vector(&env.error_messages);
    for_vector(env.inter.instructions, i, 0) {
        dfs(env.inter.instructions[i], free_node);
    }
    free_vector(&env.inter.instructions);
}

scope make_scope(void) {
    return make_vector();
}

void scope_push(scope* s, object* obj) {
    ASSERT_MSG(s != NULL, "invalid scope");
    ASSERT_MSG(obj != NULL, "scope pushing invalid obj");
    vector_pushe(*s, *obj);
    construct_object(obj);
}

void scope_pop(scope* s) {
    ASSERT_MSG(s != NULL, "invalid scope");
    object* obj = vector_back(*s);
    vector_pop(*s);
    destruct_object(obj);
}

void free_scope(scope* s) {
    ASSERT_MSG(s != NULL, "invalid scope");
    for (i32 i = (i32)vector_size(*s) - 1; i > -1; --i) {
        free_object((*s)[i]);
    }
    free_vector(s);
}

void construct_object(object* obj) {
    (void)obj;
    // NOTE: call constructor if necessary
}

void destruct_object(object* obj) {
    (void)obj;
    // NOTE: call destructor if necessary
}


