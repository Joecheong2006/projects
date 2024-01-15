#ifndef VECTOR_H
#define VECTOR_H
#include "util.h"

struct vector_status {
    u64 size, capacity;
};

#define vector(type) type*

#define init_vector(vector)\
    _init_vector((void**)(vector))

#define vector_status(vector)\
    (((struct vector_status*)(*(vector)))[-1])

#define vector_add(vector, value)\
    _vector_add(&(vector), sizeof(**(vector)));\
    (*(vector))[vector_status(vector).size++] = value;\

#define vector_pop(vector)\
    --vector_status(vector).size;

#define vector_size(vector)\
    (vector_status(vector).size)

#define vector_capacity(vector)\
    (vector_status(vector).capacity)

#define free_vector(vector)\
    _free_vector((void**)(vector));

void _init_vector(void** vector);
void _vector_reserve(void** vector, u64 newCapacity);
void _vector_add(void** vector, u64 itemSize);
void _free_vector(void** vector);

#endif
