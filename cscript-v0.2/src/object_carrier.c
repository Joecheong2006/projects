#include "object_carrier.h"
#include "core/memory.h"
#include "object.h"

object_carrier* make_object_carrier(cstring name, object* obj) {
    object_carrier* carrier = new_mem(sizeof(object_carrier));
    carrier->name = name;
    carrier->obj = obj;
    carrier->level = -1;
    return carrier;
}
