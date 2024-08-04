#include "line_renderer.h"
#include "opengl/opengl_buffer_object.h"
#include "shader_program.h"
#include "camera.h"
#include "game_object_system.h"
#include <glad/glad.h>

static vertex_array vao;
static u32 shader;

void init_debug_line_renderer() {
    glLineWidth(1);
    GLC(init_vertex_array(&vao));
    shader_program program = parse_shader("res/shaders/line.shader");
    GLC(shader = create_shader(program.vertex, program.fragment));
    shader_program_free(&program);
}

void shutdown_debug_line_renderer() {
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &vao.id);
}

void render_debug_line(vec3 p1, vec3 p2, vec3 color) {
    GLC(glUseProgram(shader));
    GLC(glBindVertexArray(vao.id));

    GLC(int location = glGetUniformLocation(shader, "color"));
    GLC(glUniform3fv(location, 1, color));

    GLC(location = glGetUniformLocation(shader, "points[0]"));
    GLC(glUniform3fv(location, 1, p1));
    GLC(location = glGetUniformLocation(shader, "points[1]"));
    GLC(glUniform3fv(location, 1, p2));

    camera* cam = find_game_object_by_index(0)->self;
    mat4 m;
    glm_mat4_mul(cam->projection, cam->view, m);
    GLC(location = glGetUniformLocation(shader, "view"));
    GLC(glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]));

    GLC(glDrawArrays(GL_LINE_STRIP, 0, 2));
}
