#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "container/hashmap.h"
#include "object.h"

typedef struct {
    object* obj;
} object_carrier;

typedef vector(object_carrier*) scope;
typedef vector(scope) scopes;

#define make_scope() make_vector(object_carrier*)
#define make_scopes() make_vector(scope)

struct environment {
    hashmap map;
    scopes global;
};
typedef struct environment environment;

object_carrier* make_object_carrier(object* obj);

void init_environment(environment* env);
i32 get_env_level(environment* env);
void free_environment(environment* env);

void env_push_scope(environment* env);
void env_pop_scope(environment* env);

object_carrier* env_find_object(environment* env, cstring name);
void env_push_object(environment* env, object_carrier* obj);

#endif
