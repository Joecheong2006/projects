#ifndef _OPENGL_OBJECT_
#define _OPENGL_OBJECT_
#include "error_type.h"
#include "util.h"

#if defined(DEBUG)
#define GLC(statement)\
    gl_clear_error();\
    statement;\
    gl_check_error(__FILE__, __LINE__);
#else
#define GLC(statement) statement
#endif

void gl_clear_error();
void gl_check_error(char*file, int line);

typedef struct {
    f32* vertices;
    u32 id;
    i32 row, col;
} vertex_buffer;

error_type init_vertex_buffer(vertex_buffer* vbo, i32 row, i32 col, u32 usage);

typedef struct {
    u32* index;
    u32 id;
} index_buffer;

error_type init_index_buffer(index_buffer* ibo, int count, u32 usage);

typedef struct {
    u32 id;
    i32 attribute_count;
    i64 offset_count;
} vertex_array;

error_type init_vertex_array(vertex_array* vao);
error_type vertex_array_add_attribute(vertex_array* vao, vertex_buffer* vbo, i32 size, u32 data_type);

typedef enum {
    TextureFilterUnkown,
    TextureFilterLinear,
    TextureFilterNearest,
} TextureFilter;

typedef struct {
    u32 id;
    TextureFilter filter;
    char* tex_path;
} texture;
error_type init_texture(texture* tex, char* texture_path, TextureFilter filter);

#endif
