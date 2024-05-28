#include "environment.h"

struct _environment env;

static u64 hash_object(void* data, u64 size) {
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
}

void delete_environment(void) {
    for_vector(env.scopes, i, 0) {
        free_scope(&env.scopes[i]);
    }
    free_vector(&env.scopes);
    free_hashmap(&env.object_map);
}

scope make_scope(void) {
    return make_vector();
}

void scope_push(scope* s, object* obj) {
    vector_pushe(*s, *obj);
    construct_object(obj);
}

void scope_pop(scope* s) {
    object* obj = vector_back(*s);
    vector_pop(*s);
    destruct_object(obj);
}

void free_scope(scope* s) {
    for_vector(*s, i, 0) {
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


