#include "string.h"
#include <string.h>
#include <stdio.h>


i32 string_init(string* self)
{
    i32 ret = vector_init(self);
    (*self)[0] = '\0';
    return ret;
}

i32 string_push(string* self, const char* string)
{
    if(string == NULL) return 0;

    u64 sl = strlen(string)+1;
    i32 ret = _vector_reserve((void**)self, vector_status(self).size+sl);
    if(!ret) return 0;

    u64 psize = vector_status(self).size;

    for(u64 i = psize; i < psize+sl; i++)
        (*self)[i] = string[i-psize];

    vector_status(self).size += sl-1;
    (*self)[vector_status(self).size] = '\0';
    return ret;
}

void string_free(string* self)
{
    vector_free(self);
}
