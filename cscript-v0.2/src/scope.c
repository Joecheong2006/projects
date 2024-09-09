#include "scope.h"
#include "core/memory.h"
#include "environment.h"

object_carrier* make_object_carrier(cstring name, object* obj) {
    object_carrier* carrier = new_mem(sizeof(object_carrier));
    carrier->name = name;
    carrier->obj = obj;
    carrier->level = -1;
    return carrier;
}

void free_scope(struct environment* env, scope sc) {
    for (i64 i = (i64)vector_size(sc) - 1; i > -1; --i) {
        vector(void*) result = hashmap_access_vector(&env->map, sc[i]);
        vector_pop(result);
        if (sc[i]->obj) {
            sc[i]->obj->destroy(sc[i]->obj, env);
        }
        free_mem(sc[i]);
    }
    free_vector(sc);
}
