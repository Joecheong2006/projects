#ifndef _SCOPE_H_
#define _SCOPE_H_
#include "container/vector.h"

struct object_carrier;
typedef vector(struct object_carrier*) scope;
#define make_scope() make_vector(object_carrier*)

struct environment;
void free_scope(struct environment* env, scope sc);

#endif
