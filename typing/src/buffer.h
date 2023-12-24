#ifndef _BUFFER_H
#define _BUFFER_H
#include <stdio.h>
#include <assert.h>
#include <wchar.h>

#ifdef DEBUG
    extern size_t allocated;
    #define ASSERT(x) assert(x)
    #define MALLOC(x)\
        malloc(x);\
        ++allocated;\
        printf("allocated = %llu\n", allocated);
    #define FREE(x)\
        ASSERT(x);\
        free(x);\
        --allocated;\
        printf("allocated = %llu\n", allocated);
#else
    #define ASSERT(x)
    #define MALLOC(x) malloc(x)
    #define FREE(x) free(x)
#endif

typedef wchar_t buffer_type;
typedef buffer_type* buffer;

buffer buffer_create(size_t size);
buffer string_create(buffer str);
buffer buffer_find_wordp(buffer buf, buffer word);
buffer buffer_find_next_worde(buffer bufp);
buffer buffer_find_next_words(buffer bufp);
void buffer_free(buffer buf);

buffer read_file(char* path);

#endif
