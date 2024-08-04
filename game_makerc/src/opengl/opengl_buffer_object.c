#include "opengl_buffer_object.h"
#include <glad/glad.h>
#include "stb_image.h"
#include <stdio.h>

void gl_clear_error()
{
    while(glGetError() != GL_NO_ERROR);
}

void gl_check_error(char*file, i32 line)
{
    GLenum error;
    while((error = glGetError())) {
        printf("error[%s:%d:%d]\n", file, line, error);
    }
}

// #define GL_BYTE 0x1400
// #define GL_UNSIGNED_BYTE 0x1401
// #define GL_SHORT 0x1402
// #define GL_UNSIGNED_SHORT 0x1403
// #define GL_INT 0x1404
// #define GL_UNSIGNED_INT 0x1405
// #define GL_FLOAT 0x1406
#define GL_FIRST_TYPE GL_BYTE

static u32 primitive_type_size[] = {
    [GL_BYTE - GL_FIRST_TYPE] = sizeof(i8),
    [GL_UNSIGNED_BYTE - GL_FIRST_TYPE] = sizeof(u8),
    [GL_SHORT - GL_FIRST_TYPE] = sizeof(i16),
    [GL_UNSIGNED_SHORT - GL_FIRST_TYPE] = sizeof(u16),
    [GL_INT - GL_FIRST_TYPE] = sizeof(i32),
    [GL_UNSIGNED_INT - GL_FIRST_TYPE] = sizeof(u32),
    [GL_FLOAT - GL_FIRST_TYPE] = sizeof(f32),
};

static error_type init_buffer_object(u32* id, void* data, u32 type, i32 size, u32 usage) {
    glGenBuffers(1, id);
    glBindBuffer(type, *id);
    glBufferData(type, size, data, usage);
    return ErrorNone;
}

error_type init_vertex_buffer(vertex_buffer* vbo, u32 count, u32 usage) {
    ASSERT(vbo != NULL && vbo->vertices != NULL);
    return init_buffer_object(&vbo->id, vbo->vertices, GL_ARRAY_BUFFER, sizeof(f32) * count, usage);
}

error_type init_index_buffer(index_buffer* ibo, i32 count, u32 usage) {
    ASSERT(ibo != NULL && ibo->index != NULL);
    ibo->count = count;
    return init_buffer_object(&ibo->id, ibo->index, GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * count, usage);
}

error_type init_vertex_array(vertex_array* vao) {
    ASSERT(vao != NULL);
    glGenVertexArrays(1, &vao->id);
    glBindVertexArray(vao->id);
    vao->attribute_count = 0;
    vao->offset_count = 0;
    return ErrorNone;
}

error_type vertex_array_add_attribute(vertex_array* vao, i32 size, u32 data_type) {
    ASSERT(vao != NULL);
    glVertexAttribPointer(vao->attribute_count, size, data_type, GL_FALSE, vao->stride, (void*)vao->offset_count);
    glEnableVertexAttribArray(vao->attribute_count++);
    vao->offset_count += primitive_type_size[data_type - GL_FIRST_TYPE] * size;
    return ErrorNone;
}

error_type init_texture(texture* tex, char* texture_path, TextureFilter filter) {
    ASSERT(tex != NULL && texture_path != NULL);
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);

    GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    tex->filter = filter;
    if (filter == TextureFilterLinear) {
        GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    }
    else if (filter == TextureFilterNearest) {
        GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
        GLC(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    }
    else {
        printf("unkwon texture filter %d\n", filter);
        tex->filter = TextureFilterUnkown;
        return ErrorInvalidParam;
    }

    i32 channels;
    u8* data = stbi_load(texture_path, &tex->width, &tex->height, &channels, 0);

    GLenum internal_format = 0, data_format = 0;
    if (channels == 3) {
        internal_format = GL_RGB8;
        data_format = GL_RGB;
    }
    else if (channels == 4) {
        internal_format = GL_RGBA8;
        data_format = GL_RGBA;
    }
    else {
        return ErrorInvalidFormat;
    }

    ASSERT_MSG(internal_format && data_format, "invalid texture channels");

    if (data) {
        GLC(glTexImage2D(GL_TEXTURE_2D, 0, internal_format, tex->width, tex->height, 0, data_format, GL_UNSIGNED_BYTE, data));
        GLC(glGenerateMipmap(GL_TEXTURE_2D));
        printf("%s %d %d\n", texture_path, tex->width, tex->height);
    }
    else {
        printf("Failed to load texture\n");
        return ErrorLoadFile;
    }

    stbi_image_free(data);

    tex->tex_path = texture_path;

    return ErrorNone;
}

