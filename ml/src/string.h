#ifndef STRING_H
#define STRING_H
#include "vector.h"


typedef vector(char) string;

i32 string_init(string*);
i32 string_push(string*, const char* string);
void string_free(string*);

#endif
