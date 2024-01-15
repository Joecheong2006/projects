#include "vector.h"
#include <stdlib.h>

void _init_vector(void** vector, u64 itemSize) {
    *vector = malloc(sizeof(struct vector_status));
    assert(*vector != NULL);
    *vector += sizeof(struct vector_status);
    vector_status(vector) = (struct vector_status){ 0, 0, itemSize };
}

void _vector_reserve(void** vector, u64 newCapacity) {
    void* newMem = realloc(&vector_status(vector), newCapacity * vector_itemSize(vector) + sizeof(struct vector_status));
    assert(newMem != NULL);
    *vector = newMem + sizeof(struct vector_status);
    vector_capacity(vector) = newCapacity;
}

void _vector_add(void** vector) {
    struct vector_status* status = &vector_status(vector);
    if (status->size + 1 <= status->capacity)
        return;
    _vector_reserve(vector, (status->capacity == 0) ? 1 : status->capacity << 1);
}

void _free_vector(void** vector) {
    assert(*vector != NULL);
    free(&vector_status(vector));
    *vector = NULL;
}

