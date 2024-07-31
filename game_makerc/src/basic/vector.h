#ifndef VECTOR_H
#define VECTOR_H

#include "util.h"

struct vector_data
{
    u64 size;
    u64 capacity;
};

#define vector(T) T*

#define make_vector()\
    _make_vector()

#define vector_status(vec)\
    (((struct vector_data*)(vec))[-1])

#define vector_size(vec)\
    (vector_status(vec).size)


#define vector_capacity(vec)\
    (vector_status(vec).capacity)


#define vector_push(vec, ...) {\
        vec = _vector_add(vec, sizeof(__typeof__(*(vec))));\
        _vector_new_value(vec, &(__typeof__(*(vec))){__VA_ARGS__}, sizeof(__typeof__(*(vec))));\
    }

#define vector_pushe(vec, ...) {\
        vec = _vector_add(vec, sizeof(__typeof__(*(vec))));\
        _vector_new_value(vec, &(__VA_ARGS__), sizeof(__typeof__(*(vec))));\
    }

#define vector_reserve(vec, reserve_size)\
    vec = _vector_reserve(vec, reserve_size * sizeof(__typeof(*(vec))))

#define vector_resize(vec, new_size) {\
        vector_reserve(vec, new_size);\
        vector_size(vec) = new_size;\
    }

#define vector_copy(dest, src) {\
        u64 src_len = vector_size(src);\
        for (u64 i = 0; i < src_len; i++) {\
            vector_push(dest, src[i]);\
        }\
    }

#define vector_back(vec) vec[vector_size(vec) - 1]

#define for_vector(vec, i, start_index) for (u64 i = start_index; i < vector_size(vec); ++i)

#define vector_pop(vec)\
    vector_status(vec).size--

#define free_vector(vec)\
    _free_vector((void*)(vec))


void* _make_vector(void);
void* _vector_reserve(void* vec, u64 size);
void* _vector_add(void* vec, u64 size);
void _vector_new_value(void* vec, void* data, u64 size);
void _free_vector(void* vec);

#endif

