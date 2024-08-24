#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "container/hashmap.h"
#include "object.h"

typedef vector(object*) scope;
typedef vector(scope) scopes;

#define make_scope() make_vector(object*)
#define make_scopes() make_vector(scope)

void setup_global_env(void);
void shutdown_global_env(void);

void scopes_push(void);
void scopes_pop(void);

object* find_object(cstring name);
void push_object(object* obj);
void pop_object(void);

#endif
