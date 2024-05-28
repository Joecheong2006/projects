#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include "basic/hashmap.h"
#include "object.h"

typedef vector(object*) scope;

struct _environment {
    hashmap object_map;
    vector(scope) scopes;
};

extern struct _environment env;

void init_environment(void);
void delete_environment(void);

scope make_scope(void);
void scope_push(scope* s, object* obj);
void scope_pop(scope* s);
void free_scope(scope* s);

void construct_object(object* obj);
void destruct_object(object* obj);

#endif
