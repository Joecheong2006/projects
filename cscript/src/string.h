#ifndef STRING_H
#define STRING_H
#include "vector.h"

typedef vector(char) string;

#define string_push(str, ctx)\
    _string_push(&str, ctx);

#define make_string(ctx)\
    _make_string(ctx)

string _make_string(const string ctx);
void _string_push(string*, const string string);
void free_string(string);

#endif
