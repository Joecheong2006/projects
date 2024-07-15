#ifndef _SPRITE_
#define _SPRITE_
#include "opengl_object.h"
#include "camera2d.h"
#include <cglm/cglm.h>

typedef struct {
    vec2 position;
    vec2 scale;
} transform2d;

typedef struct {
    vec2 per_sprite;
    texture tex;
} sprite_texture;

typedef struct {
    transform2d tran;
    vec2 sprite_index;
} sprite;

extern struct sprite_instance {
    vertex_array vao;
    vertex_buffer vbo;
    index_buffer ibo;
    u32 shader;
} sprite_instance;

void init_sprite_instance();
void render_sprite(camera2d* cam, sprite_texture* sprite_tex, sprite* sp);

#endif
