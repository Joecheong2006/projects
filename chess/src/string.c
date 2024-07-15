#include "string.h"
#include <string.h>
#include <assert.h>

string _make_string(const string ctx)
{
    string result = make_vector();
    string_push(result, ctx);
    return result;
}

string _make_stringn(const string ctx, u64 len) {
    string result = make_vector();
    char str[len + 1];
    memcpy(str, ctx, len);
    str[len] = 0;
    string_push(result, str);
    return result;
}

void _string_push(string* dest, const string string)
{
    u64 sl = strlen(string) + 1;
    *dest = _vector_reserve((*dest), vector_status(*dest).size + sl);

    u64 psize = vector_status(*dest).size;

    for(u64 i = psize; i < psize + sl; i++)
        (*dest)[i] = string[i - psize];

    vector_status(*dest).size += sl - 1;
    (*dest)[vector_status(*dest).size] = '\0';
}

void free_string(string str)
{
    free_vector(str);
}
