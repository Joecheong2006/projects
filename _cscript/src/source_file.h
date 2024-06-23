#ifndef SOURCE_FILE_H
#define SOURCE_FILE_H
#include "basic/util.h"

typedef struct {
    char* buffer;
    u32 buffer_size, line_count;
} source_file;

i32 load_source(source_file* source, const char* path);
void free_source(source_file* source);

#endif
