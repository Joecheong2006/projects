#ifndef _VECTOR_H
#define _VECTOR_H
#include "util.h"


struct vector_data
{
    u64 size;
    u64 capacity;
};

#define vector(T) T*


#define vector_init(vec)\
    _vector_init((void**)(vec))\


#define vector_status(vec)\
    (((struct vector_data*)(*(vec)))[-1])\
    //(*(struct vector_data*)((void*)(*(vec))-sizeof(struct vector_data)))

#define vector_size(vec)\
    (vector_status(vec).size)


#define vector_capacity(vec)\
    (vector_status(vec).capacity)


#define vector_push(vec, value)\
    if(_vector_add((void**)(vec), sizeof(**(vec))))\
        (*(vec))[vector_status(vec).size++] = value;\


#define vector_pop(vec)\
    vector_status(vec).size--;


#define vector_free(vec)\
    _vector_free((void**)(vec))\


i32 _vector_init(void** vec);
i32 _vector_reserve(void** vec, u64 size);
i32 _vector_add(void** vec, u64 msize);
void _vector_free(void** vec);

#endif

