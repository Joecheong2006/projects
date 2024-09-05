#include "scope.h"
#include "container/memallocate.h"
#include "environment.h"

object_carrier* make_object_carrier(object* obj) {
    object_carrier* carrier = MALLOC(sizeof(object_carrier));
    carrier->obj = obj;
    return carrier;
}

void free_scope(struct environment* env, scope sc) {
    for (i64 i = (i64)vector_size(sc) - 1; i > -1; --i) {
        vector(void*) result = hashmap_access_vector(&env->map, sc[i]);
        vector_pop(result);
        sc[i]->obj->destroy(sc[i]->obj, env);
        FREE(sc[i]);
    }
    free_vector(sc);
}
