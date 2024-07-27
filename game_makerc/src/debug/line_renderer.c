#include "line_renderer.h"
#include "../opengl_object.h"
#include "../shader_program.h"
#include "../camera.h"
#include "../game_object_system.h"
#include <glad/glad.h>

static vertex_array vao;
static u32 shader;

void init_debug_line_renderer_instance() {
    glLineWidth(2);
    GLC(init_vertex_array(&vao));
    shader_program program = parse_shader("res/shaders/line.shader");
    GLC(shader = create_shader(program.vertex, program.fragment));
    shader_program_free(&program);
}

void render_debug_line(float points[6], vec3 color) {
    ASSERT(points != NULL && color != NULL);
    GLC(glUseProgram(shader));
    GLC(glBindVertexArray(vao.id));

    GLC(int location = glGetUniformLocation(shader, "color"));
    GLC(glUniform3fv(location, 1, color));

    GLC(location = glGetUniformLocation(shader, "points"));
    GLC(glUniform3fv(location, 2, points));

    camera* cam = find_game_object_by_index(0)->self;
    mat4 m;
    glm_mat4_mul(cam->projection, cam->view, m);
    GLC(location = glGetUniformLocation(shader, "view"));
    GLC(glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]));

    GLC(glDrawArrays(GL_LINE_STRIP, 0, 2));
}
