#include "Circle.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define MAX_SIZE 1024 * 8

static f32 vertexs[] = {
        -1.0f, -1.0f, 0.0, 0.0,
        1.0f, -1.0f,  1.0, 0.0,
        -1.0f, 1.0f,  0.0, 1.0,

        1.0f, 1.0f,   1.0, 1.0,
        -1.0f, 1.0f,  1.0, 0.0,
        1.0f, -1.0f,  0.0, 1.0,
};

namespace mfw {
    Circle::Manager::Manager()
        : m_vao(), m_vbo(vertexs, sizeof(vertexs)), m_shader(), m_texture("res/images/circle.png")
    {
        VertexBufferLayout cube_layout;
        cube_layout.add<float>(2);
        cube_layout.add<float>(2);
        m_vao.applyBufferLayout(cube_layout);

        m_shader.attachShader(GL_VERTEX_SHADER, "res/shaders/default_sprite.vert");
        m_shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/default_sprite.frag");
        m_shader.link();

        m_texture.bind();

        m_vao.unbind();
        m_vbo.unbind();
        m_shader.unbind();
        m_texture.unbind();

        entities.reserve(MAX_SIZE);
    }

    Circle& Circle::Manager::createCircle(const glm::vec2& pos, const glm::vec3& color, const f32& d)
    {
        entities.push_back(Circle(pos, color, d));
        return entities.back();
    }

    Circle& Circle::Manager::createCircle(const f32& d) {
        entities.push_back(Circle(glm::vec2(0), glm::vec3(1), d));
        return entities.back();
    }


    void Circle::Manager::renderCircle(const glm::mat4& o) {
        m_vao.bind();
        m_texture.bind();
        m_shader.bind();
        m_shader.set1i("tex", 0);
        glm::mat4 view;
        for (auto& e : entities) {
            view = glm::mat4(1);
            view = glm::translate(view, glm::vec3(e.m_pos.x, e.m_pos.y, 0));
            view = glm::scale(view, glm::vec3(e.d, e.d, 0));
            m_shader.set3f("color", glm::value_ptr(e.m_color));
            m_shader.setMat4("view", o * view);
            GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
        }
        m_texture.unbind();
        m_shader.unbind();
        m_vao.unbind();
    }

    Circle::Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d)
        : Object2D(pos, d * 1.25, color), d(d)
    {
    }

    bool Circle::collide(Circle& c) {
        if(&c == this) return false;
        return (m_pos.x - c.m_pos.x) * (m_pos.x - c.m_pos.x) + (m_pos.y - c.m_pos.y) * (m_pos.y - c.m_pos.y) < (d + c.d) * (d + c.d);
    }

    void Circle::solveCollision(Circle& c) {
        f32 d = glm::length(m_pos - c.m_pos);
        glm::vec2 di = 0.5f * (this->d + c.d - d) * (m_pos - c.m_pos) / d;
        m_pos += di;
        c.m_pos -= di;
    }
}

