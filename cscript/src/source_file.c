#include "source_file.h"
#include "memallocate.h"
#include <assert.h>
#include <string.h>

#define MAX_LINE_BUFFER_SIZE (1 << 10)
#define MAX_WORD_BUFFER_SIZE (1 << 8)

i32 is_space_strn(const char* str, u64 size) {
    for (u64 i = 0; i < size; ++i) {
        if (str[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

i32 load_source(source_file* source, const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("cannot fount %s\n", path);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    source->buffer_size = ftell(file);
    rewind(file);

    source->buffer = MALLOC(source->buffer_size + 1);

    u64 line_count = 0, count = 0;
    char line_buffer[MAX_LINE_BUFFER_SIZE];

    while (fgets(line_buffer, MAX_LINE_BUFFER_SIZE, file)) {
        u64 line_len = strlen(line_buffer);
        memcpy(source->buffer + count, line_buffer, line_len);
        count += line_len;
        ++line_count;
    }

    source->buffer_size = count;
    source->line_count = line_count;

    source->buffer[source->buffer_size] = 0;

    fclose(file);

    return 1;
}

void free_source(source_file* source) {
    assert(source->buffer);
    FREE(source->buffer);
    memset(source, 0, sizeof(source_file));
}

