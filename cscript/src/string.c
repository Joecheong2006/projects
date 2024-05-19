#include "string.h"
#include <string.h>
#include <assert.h>


string _make_string(const string ctx)
{
    string result = make_vector();
    string_push(result, ctx);
    return result;
}

void _string_push(string* self, const string string)
{
    u64 sl = strlen(string) + 1;
    *self = _vector_reserve((*self), vector_status(*self).size + sl);

    u64 psize = vector_status(*self).size;

    for(u64 i = psize; i < psize + sl; i++)
        (*self)[i] = string[i - psize];

    vector_status(*self).size += sl - 1;
    (*self)[vector_status(*self).size] = '\0';
}

void free_string(string self)
{
    free_vector(self);
}
