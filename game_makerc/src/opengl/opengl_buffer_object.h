#ifndef _OPENGL_BUFFER_OBJECT_
#define _OPENGL_BUFFER_OBJECT_
#include "error_type.h"
#include "core/util.h"

#if defined(DEBUG)
#define GLC(statement)\
    gl_clear_error();\
    statement;\
    gl_check_error(__FILE__, __LINE__);
#else
#define GLC(statement) statement
#endif

void gl_clear_error();
void gl_check_error(char*file, i32 line);

typedef struct {
    f32* vertices;
    u32 id;
} vertex_buffer;

error_type init_vertex_buffer(vertex_buffer* vbo, u32 count, u32 usage);

typedef struct {
    u32* index;
    u32 count;
    u32 id;
} index_buffer;

error_type init_index_buffer(index_buffer* ibo, i32 count, u32 usage);

typedef struct {
    u32 id;
    i32 attribute_count, stride;
    i64 offset_count;
} vertex_array;

error_type init_vertex_array(vertex_array* vao);
error_type vertex_array_add_attribute(vertex_array* vao, i32 size, u32 data_type);

typedef enum {
    TextureFilterUnkown,
    TextureFilterLinear,
    TextureFilterNearest,
} TextureFilter;

typedef struct {
    u32 id;
    TextureFilter filter;
    i32 width, height;
    char* tex_path;
} texture;
error_type init_texture(texture* tex, char* texture_path, TextureFilter filter);

#endif
