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

    shader_program shader = parse_shader("res/shaders/sprite.shader");
    GLC(sprite_instance.shader = create_shader(shader.vertex, shader.fragment));
    shader_program_free(&shader);

    glBindVertexArray(0);
}

void render_sprite(camera* cam,  transform* tran, sprite_texture* sprite_tex, sprite* sp) {
    ASSERT(cam != NULL && tran != NULL && sprite_tex != NULL);
	GLC(glUseProgram(sprite_instance.shader));
	GLC(glBindVertexArray(sprite_instance.vao.id));

	glActiveTexture(GL_TEXTURE0);
	GLC(glBindTexture(GL_TEXTURE_2D, sprite_tex->tex.id));

    int location;
    // GLC(location = glGetUniformLocation(sprite_instance.shader, "tex"));
    // GLC(glUniform1i(location, 0));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "per_sprite"));
    GLC(glUniform2f(location, sprite_tex->per_sprite[0], sprite_tex->per_sprite[1]));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "sprite_index"));
    GLC(glUniform2f(location, sp->sprite_index[0], sp->sprite_index[1]));
    GLC(location = glGetUniformLocation(sprite_instance.shader, "color"));
    GLC(glUniform4fv(location, 1, sp->color));

    mat4 m, trans, scale;
    glm_mat4_identity(trans);
    if (tran->parent) {
        glm_vec3_copy(tran->local_position, tran->position);
        glm_vec3_add(tran->position, tran->parent->position, tran->position);
    }
    glm_translate(trans, tran->position);

    glm_mat4_identity(scale);
    glm_scale(scale, tran->scale);

    glm_mat4_mul(cam->projection, cam->view, m);
    glm_mat4_mul(m, trans, m);
    glm_mat4_mul(m, scale, m);

    GLC(location = glGetUniformLocation(sprite_instance.shader, "view"));
    GLC(glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]));
    
    GLC(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	// glBindTexture(GL_TEXTURE_2D, 0);
	// glBindVertexArray(0);
	// glUseProgram(0);
}
