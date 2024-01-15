#include "vector.h"
#include <stdlib.h>
#include <stdio.h>

void _init_vector(void** vector) {
    *vector = malloc(sizeof(struct vector_status));
    assert(*vector != NULL);
    *vector += sizeof(struct vector_status);
    vector_status(vector) = (struct vector_status){ 0, 0 };
}

void _vector_reserve(void** vector, u64 newCapacity) {
    void* newMem = realloc(&vector_status(vector), newCapacity + sizeof(struct vector_status));
    assert(!newMem);
    *vector = newMem + sizeof(struct vector_status);
    vector_status(vector).capacity = newCapacity;
}

void _vector_add(void** vector, u64 itemSize) {
    struct vector_status* status = &vector_status(vector);
    if ((status->size + 1) * itemSize > status->capacity) {
        _vector_reserve(vector, (status->size + 1) * itemSize);
    }
    ++status->size;
}

void _free_vector(void** vector) {
    assert(*vector != NULL);
    free(&vector_status(vector));
    *vector = NULL;
}

