#ifndef STRING_H
#define STRING_H
#include "vector.h"

typedef vector(char) string;

#define string_push(str, ctx)\
    _string_push(&str, ctx);

#define make_string(ctx)\
    _make_string(ctx)

#define make_stringn(ctx, len)\
    _make_stringn(ctx, len)

string _make_string(const string ctx);
string _make_stringn(const string ctx, u64 len);
void _string_push(string* dest, const string string);
void free_string(string str);

#endif
