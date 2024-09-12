#ifndef _OBJECT_CARRIER_H_
#define _OBJECT_CARRIER_H_
#include "container/string.h"

struct object;
struct object_carrier {
    cstring name;
    i32 level;
    struct object* obj;
};
typedef struct object_carrier object_carrier;
object_carrier* make_object_carrier(cstring name, struct object* obj);

#endif
