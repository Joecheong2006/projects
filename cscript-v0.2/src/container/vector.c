#include "vector.h"
#include "core/memory.h"
#include "core/assert.h"
#include <string.h>

void* _make_vector(u64 type_size)
{
    char* val = (char*)new_mem(sizeof(struct vector_data));
    ASSERT(val != NULL);
    val += (u64)sizeof(struct vector_data);
    vector_status(val) = (struct vector_data){ 0, 0, type_size };
    return val;
}

void* _vector_reserve(void* vec, u64 size)
{
    if (vector_capacity(vec) > size) {
        return vec;
    }
    char* result = (char*)renew_mem(&vector_status(vec), size + sizeof(struct vector_data));
    ASSERT(result != NULL);
    result += sizeof(struct vector_data);
    vector_status(result).capacity = size;
    return result;
}

void* _vector_add(void* vec)
{
    u64 size = vector_status(vec).type_size;
    struct vector_data* data = &vector_status(vec);

    while((data->size + 1) * size > data->capacity)
       data->capacity = ((data->capacity == 0) ? 1 : data->capacity << 1);

    ++data->size;
    return _vector_reserve(vec, data->capacity);
}

void _vector_new_value(void* vec, void* data)
{
    u64 size = vector_status(vec).type_size;
    memcpy(((char*)vec) + (vector_size(vec) - 1) * size, data, size);
}

void _free_vector(void* vec)
{
    ASSERT(vec != NULL);
    free_mem(&vector_status(vec));
}

