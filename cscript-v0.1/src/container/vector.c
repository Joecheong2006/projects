#include "vector.h"
#include "memallocate.h"
#include <string.h>
#include <assert.h>

void* _make_vector(void)
{
    char* val = (char*)MALLOC(sizeof(struct vector_data));
    assert(val != NULL);
    val += (u64)sizeof(struct vector_data);
    vector_status(val) = (struct vector_data){ 0, 0 };
    return val;
}

void* _vector_reserve(void* vec, u64 size)
{
    char* result = (char*)realloc(&vector_status(vec), size + sizeof(struct vector_data));
    assert(result != NULL);
    result += sizeof(struct vector_data);
    vector_status(result).capacity = size;
    return result;
}

void* _vector_add(void* vec, u64 size)
{
    struct vector_data* data = &vector_status(vec);

    while((data->size + 1) * size > data->capacity)
       data->capacity = ((data->capacity == 0) ? 1 : data->capacity << 1);

    ++data->size;
    return _vector_reserve(vec, data->capacity);
}

void _vector_new_value(void* vec, void* data, u64 size)
{
    memcpy(((char*)vec) + (vector_size(vec) - 1) * size, data, size);
}

void _free_vector(void* vec)
{
    assert(vec != NULL);
    FREE(&vector_status(vec));
}

