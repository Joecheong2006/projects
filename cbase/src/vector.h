#ifndef VECTOR_H
#define VECTOR_H
#include "util.h"

struct vector_status {
    u64 size, capacity, itemSize;
};

#define vector(type) type*

#define init_vector(vector)\
    _init_vector((void**)(vector), sizeof(**(vector)))

#define vector_reserve(vector, newCapacity)\
    _vector_reserve((void**)(vector), newCapacity)

#define vector_status(vector)\
    (((struct vector_status*)(*(vector)))[-1])

#define vector_add(vector, value)\
    _vector_add((void**)(vector));\
    (*(vector))[vector_status(vector).size++] = value;\

#define vector_pop(vector)\
    (--vector_status(vector).size)

#define vector_size(vector)\
    (vector_status(vector).size)

#define vector_capacity(vector)\
    (vector_status(vector).capacity)

#define vector_itemSize(vector)\
    (vector_status(vector).itemSize)

#define vector_clear(vector)\
    (vector_status(vector).size = 0)

#define free_vector(vector)\
    _free_vector((void**)(vector));

void _init_vector(void** vector, u64 itemSize);
void _vector_reserve(void** vector, u64 newCapacity);
void _vector_add(void** vector);
void _free_vector(void** vector);

#endif
