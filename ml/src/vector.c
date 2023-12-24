#include "vector.h"
#include <stdlib.h>
#include <stdio.h>


i32 _vector_init(void** val)
{
    *val = malloc(sizeof(struct vector_data))+sizeof(struct vector_data);
    vector_status(val) = (struct vector_data){ 0, 0 };
    return 1;
}

i32 _vector_reserve(void** vec, u64 size)
{
    *vec = realloc(&vector_status(vec), size+sizeof(struct vector_data));
    if(*vec == NULL) return 0;
    *vec += sizeof(struct vector_data);
    vector_status(vec).capacity = size;
    return 1;
}

i32 _vector_add(void** vec, u64 msize)
{
    struct vector_data* data = &vector_status(vec);

    if((data->size+1)*msize <= data->capacity) return 1;

    while((data->size+1)*msize > data->capacity)
       data->capacity = ((data->capacity == 0) ? 1 : data->capacity << 1);

    return _vector_reserve(vec, data->capacity);
}

void _vector_free(void** vec)
{
    if(*vec == NULL) return;
    free(&vector_status(vec));
    printf("free %p\n", &vector_status(vec));
    *vec = NULL;
}


