#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H
#include "basic/util.h"

typedef struct {
    char* buffer;
    u64 buffer_size, line_count;
} source_file;

i32 is_space_strn(const char* str, u64 size);
i32 load_source(source_file* source, const char* path);
void free_source(source_file* source);

#endif
