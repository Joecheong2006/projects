#include "scope.h"
#include "object_carrier.h"
#include "core/memory.h"
#include "environment.h"

void free_scope(struct environment* env, scope sc) {
    for (i64 i = (i64)vector_size(sc) - 1; i > -1; --i) {
        vector(void*) result = hashmap_access_vector(&env->map, sc[i]);
        vector_pop(result);
        if (sc[i]->obj) {
            sc[i]->obj->destroy(sc[i]->obj);
        }
        free_mem(sc[i]);
    }
    free_vector(sc);
}
