#include "sprite.h"
#include "shader_program.h"
#include <glad/glad.h>

struct sprite_instance sprite_instance;

void init_sprite_instance() {
    GLC(init_vertex_array(&sprite_instance.vao));
    sprite_instance.vbo = (vertex_buffer){
        .vertices = (f32[]){
             0.5,  0.5,  0.0, 1.0, 1.0,
            -0.5,  0.5,  0.0, 0.0, 1.0,
             0.5, -0.5,  0.0, 1.0, 0.0,
            -0.5, -0.5,  0.0, 0.0, 0.0,
        }
    };
    GLC(init_vertex_buffer(&sprite_instance.vbo, 5, 4, GL_STATIC_DRAW));
    GLC(vertex_array_add_attribute(&sprite_instance.vao, &sprite_instance.vbo, 3, GL_FLOAT));
    GLC(vertex_array_add_attribute(&sprite_instance.vao, &sprite_instance.vbo, 2, GL_FLOAT));

    sprite_instance.ibo = (index_buffer){
        .index = (u32[]){
            0, 1, 2,
            2, 1, 3,
        }
    };
    GLC(init_index_buffer(&sprite_instance.ibo, 6, GL_STATIC_DRAW));

    shader_program shader = parse_shader("res/shaders/Basic.shader");
    GLC(sprite_instance.shader = create_shader(shader.vertex, shader.fragment));
    shader_program_free(&shader);

    glBindVertexArray(0);
}

void render_sprite(camera2d* cam, sprite_texture* sprite_tex, sprite* sp) {
	GLC(glUseProgram(sprite_instance.shader));

	glActiveTexture(GL_TEXTURE0 + sprite_tex->tex.slot);
	GLC(glBindTexture(GL_TEXTURE_2D, sprite_tex->tex.id));

    int location;
    GLC(location = glGetUniformLocation(sprite_instance.shader, "tex"));
    GLC(glUniform1i(location, sprite_tex->tex.slot));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "per_sprite"));
    GLC(glUniform2f(location, sprite_tex->per_sprite[0], sprite_tex->per_sprite[1]));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "sprite_index"));
    GLC(glUniform2f(location, sp->sprite_index[0], sp->sprite_index[1]));

    mat4 m, trans, scale;
    glm_mat4_identity(trans);
    glm_translate(trans, (vec3){sp->tran.position[0], sp->tran.position[1], 0});

    glm_mat4_identity(scale);
    glm_scale(scale, (vec3){sp->tran.scale[0], sp->tran.scale[1], 0});

    glm_mat4_identity(m);
    glm_mat4_mul(cam->ortho, cam->view, m);
    glm_mat4_mul(m, trans, m);
    glm_mat4_mul(m, scale, m);

    GLC(location = glGetUniformLocation(sprite_instance.shader, "view"));
    GLC(glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]));
    
	GLC(glBindVertexArray(sprite_instance.vao.id));
    GLC(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	glBindVertexArray(0);
	glUseProgram(0);
}
