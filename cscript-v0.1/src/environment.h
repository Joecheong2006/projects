#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "container/hashmap.h"
#include "object.h"

typedef vector(object*) scope;
typedef vector(scope) scopes;

#define make_scope() make_vector(object*)
#define make_scopes() make_vector(scope)

struct environment {
    hashmap map;
    scopes global;
};
typedef struct environment environment;

void init_environment(environment* env);
i32 get_env_level(environment* env);
void free_environment(environment* env);

void env_push_scope(environment* env);
void env_pop_scope(environment* env);

object* env_find_object(environment* env, cstring name);
void env_push_object(environment* env, object* obj);

#endif
