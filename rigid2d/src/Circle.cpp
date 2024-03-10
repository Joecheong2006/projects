#include "Circle.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static f32 vertexs[] = {
        -1.0f, -1.0f, 0.0, 0.0,
        1.0f, -1.0f,  1.0, 0.0,
        -1.0f, 1.0f,  0.0, 1.0,

        1.0f, 1.0f,   1.0, 1.0,
        -1.0f, 1.0f,  1.0, 0.0,
        1.0f, -1.0f,  0.0, 1.0,
};

namespace mfw {
    Circle::Renderer* Circle::renderer = nullptr;

    Circle::Renderer::Renderer()
        : m_vao(), m_vbo(vertexs, sizeof(vertexs)), m_shader(), m_texture("res/images/circle.png")
    {
        VertexBufferLayout cube_layout;
        cube_layout.add<f32>(2);
        cube_layout.add<f32>(2);
        m_vao.applyBufferLayout(cube_layout);

        m_shader.attachShader(GL_VERTEX_SHADER, "res/shaders/texture.vert");
        m_shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/texture.frag");
        m_shader.link();
        m_texture.bind();

        m_vao.unbind();
        m_vbo.unbind();
        m_shader.unbind();
        m_texture.unbind();
    }

    void Circle::Renderer::bind() {
        m_texture.bind();
        m_shader.bind();
        m_vao.bind();
    }

    void Circle::Renderer::unbind() {
        m_texture.unbind();
        m_shader.unbind();
        m_vao.unbind();
    }

    void Circle::Renderer::render(const glm::mat4& o, Circle& circle) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle.m_pos.x, circle.m_pos.y, 0));
        view = glm::scale(view, glm::vec3(circle.r, circle.r, 1));
        m_shader.set1i("tex", 0);
        m_shader.set4f("color", circle.m_color);
        m_shader.setMat4("view", o * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Circle::Renderer::draw(const glm::mat4& o, Circle& circle) {
        bind();
        render(o, circle);
        unbind();
    }

    Circle::Circle(const glm::vec2& pos, const glm::vec4& color, const f32& d)
        : Object2D(pos, d, color), r(d)
    {}

    Circle::Circle()
        : Object2D(glm::vec2(0), 1, glm::vec4(1)), r(1)
    {}

    bool Circle::collide(Circle& c) {
        if(&c == this) return false;
        return (m_pos.x - c.m_pos.x) * (m_pos.x - c.m_pos.x) + (m_pos.y - c.m_pos.y) * (m_pos.y - c.m_pos.y) < (r + c.r) * (r + c.r);
    }

    void Circle::solveCollision(Circle& c) {
        f32 d = glm::length(m_pos - c.m_pos);
        glm::vec2 di = 0.5f * (this->r + c.r - d) * (m_pos - c.m_pos) / d;
        m_pos += di;
        c.m_pos -= di;
    }
}

