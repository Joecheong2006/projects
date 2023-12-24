#include "buffer.h"
#include <stdlib.h>
#include <string.h>

size_t allocated;

static int str_cmpc(buffer s1, buffer s2, size_t c);
static long int get_file_size(FILE* file);

buffer buffer_create(size_t size) {
    buffer result = MALLOC(sizeof(buffer_type) * size);
    ASSERT(result != NULL);
    return result;
}

buffer string_create(buffer str) {
    int len = wcslen(str) + 1;
    buffer result = MALLOC(len);
    wcscpy(result, str);
    result[len-1] = 0;
    return result;
}

int str_cmpc(buffer s1, buffer s2, size_t c) {
    for(size_t i = 0; i < c; ++i)
        if(s1[i] != s2[i])
            return 0;
    return 1;
}

buffer buffer_find_wordp(buffer buf, buffer word) {
    int len = wcslen(word);
    for(int i = 0; buf[i] != 0; ++i) {
        if(buf[i] == word[0] && str_cmpc(word, (buf + i), len))
        {
            printf("find f: %c e: %c\n", buf[i], buf[i+len-1]);
            return &buf[i];
        }
    }
    return NULL;
}

buffer buffer_find_next_worde(buffer bufp) {
    for(int i = 0;; ++i)
    {
        if(bufp[i] == ' ')
        {
            return &bufp[i];
        }
    }
    return NULL;
}

buffer buffer_find_next_words(buffer bufp) {
    for(int i = 0;; ++i)
    {
        if(bufp[i] != ' ' && bufp[i] != '\n') 
        {
            return &bufp[i];
        }
    }
    return NULL;
}

void buffer_free(buffer buf) {
    ASSERT(buf != NULL);
    FREE(buf);
}

long int get_file_size(FILE* file) {
    size_t result = 0;
    do {
        ++result;
    } while(getc(file) != EOF);
    fseek(file, 0, SEEK_SET);
    return result;
}

buffer read_file(char* path) {
    FILE* file = fopen(path, "r");
    if(!file)
    {
        fclose(file);
        return NULL;
    }

    long int size = get_file_size(file);
    buffer result = buffer_create(size + 1);
    if(!result)
        return NULL;

    for(long int i = 0; i < size; ++i)
        result[i] = getc(file);
    result[size-1] = 0;

    fclose(file);
    return result;
}

