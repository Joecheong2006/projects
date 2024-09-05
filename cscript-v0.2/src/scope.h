#ifndef _SCOPE_H_
#define _SCOPE_H_
#include "container/vector.h"
#include "object.h"

struct object_carrier {
    object* obj;
};
typedef struct object_carrier object_carrier;
object_carrier* make_object_carrier(object* obj);

typedef vector(object_carrier*) scope;
#define make_scope() make_vector(object_carrier*)

struct environment;
void free_scope(struct environment* env, scope sc);

#endif
