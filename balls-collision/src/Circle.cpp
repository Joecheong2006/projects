#include "Circle.h"
#include "VertexBufferLayout.hpp"

#define MAX_SIZE 1024 * 8

static f32 vertexs[] = {
        -1.0f, -1.0f, 0.0, 0.0,
        1.0f, -1.0f,  1.0, 0.0,
        -1.0f, 1.0f,  0.0, 1.0,

        1.0f, 1.0f,   1.0, 1.0,
        -1.0f, 1.0f,  1.0, 0.0,
        1.0f, -1.0f,  0.0, 1.0,
};

Circle::Manager::Manager()
    : m_vao(), m_vbo(vertexs, sizeof(vertexs)), m_shader(), m_texture("res/images/circle.png")
{
    VertexBufferLayout cube_layout;
    cube_layout.add<float>(2);
    cube_layout.add<float>(2);
    m_vao.apply_buffer_layout(cube_layout);

    m_shader.attach_shader(GL_VERTEX_SHADER, "res/shaders/default.vert");
    m_shader.attach_shader(GL_FRAGMENT_SHADER, "res/shaders/default.frag");
    m_shader.link();

    m_texture.bind();

    entities.reserve(MAX_SIZE);
}

Circle& Circle::Manager::create_circle(const glm::vec2& pos, const glm::vec3& color, const f32& d)
{
    entities.push_back(Circle(pos, color, d));
    return entities.back();
}

Circle::Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d)
    : Object2D(pos, d * 1.25, color), d(d)
{
}

bool Circle::collide(Circle& c) {
    if(&c == this) return false;
    return glm::length(m_pos - c.m_pos) < (d + c.d);
}

void Circle::solve_collision(Circle& c) {
    f32 d = glm::length(m_pos - c.m_pos);
    glm::vec2 di = 0.5f * (this->d + c.d - d) * (m_pos - c.m_pos) / d;
    m_pos += di;
    c.m_pos -= di;
}

