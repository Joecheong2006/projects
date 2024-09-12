#ifndef _ENVIRONMENT_H_
#define _ENVIRONMENT_H_

#include "container/hashmap.h"
#include "scope.h"
#include "stack.h"
#include "object.h"

struct environment {
    hashmap map;
    vector(scope) global;
    stack bp;
};
typedef struct environment environment;

void init_environment(environment* env);
i32 get_env_level(environment* env);
void free_environment(environment* env);

void env_push_scope(environment* env);
void env_pop_scope(environment* env);

struct object_carrier* env_find_object(environment* env, cstring name);
void env_push_object(environment* env, struct object_carrier* carrier);

void env_remove_object_from_scope(environment* env, struct object_carrier* carrier);
void env_pop_object(environment* env, struct object_carrier* carrier);

#endif
