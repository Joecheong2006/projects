#ifndef _SPRITE_
#define _SPRITE_
#include "opengl_object.h"
#include "camera.h"
#include "transform.h"

typedef struct {
    vec2 per_sprite;
    texture tex;
} sprite_texture;

typedef struct {
    vec2 sprite_index;
    vec4 color;
} sprite;

extern struct sprite_instance {
    vertex_array vao;
    vertex_buffer vbo;
    index_buffer ibo;
    u32 shader;
} sprite_instance;

void init_sprite_instance();
void render_sprite(camera* cam, transform* tran, sprite_texture* sprite_tex, sprite* sp);

#endif
