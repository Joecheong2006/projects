#include "Stick.h"
#include "Renderer.h"
#include "Circle.h"
#include "VertexBufferLayout.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

static f32 vertexs[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,

        1.0f, 1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f,
};

namespace mfw {
    Stick::Renderer* Stick::renderer = nullptr;
    Stick::Attribute Stick::attribute;

    Stick::Renderer::Renderer()
        : m_vbo(vertexs, sizeof(vertexs))
    {
        VertexBufferLayout cube_layout;
        cube_layout.add<float>(2);
        m_vao.applyBufferLayout(cube_layout);

        m_shader.attachShader(GL_VERTEX_SHADER, "res/shaders/line.vert");
        m_shader.attachShader(GL_FRAGMENT_SHADER, "res/shaders/line.frag");
        m_shader.link();

        m_vao.unbind();
        m_vbo.unbind();
        m_shader.unbind();
    }

    void Stick::Renderer::bind() {
        m_vao.bind();
        m_shader.bind();
    }

    void Stick::Renderer::unbind() {
        m_vao.unbind();
        m_shader.unbind();
    }


    void Stick::Renderer::render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3((p1 + p2) * 0.5f, 0));
        view = glm::rotate(view, glm::atan((p1.y - p2.y) / (p1.x - p2.x)), glm::vec3(0, 0, 1));
        view = glm::scale(view, glm::vec3(glm::length(p1 - p2) * 0.5, w, 0));
        m_shader.set3f("color", glm::value_ptr(color));
        m_shader.setMat4("view", o * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Stick::Renderer::draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        bind();
        render(o, p1, p2, color, w);
        unbind();
    }

    Stick::Stick(glm::vec2* p1, glm::vec2* p2, f32 d, Attribute& attribute)
        : d(d), attri(attribute)
    {
        p[0] = p1;
        p[1] = p2;
    }

    void Stick::update() {
        f32 cd = glm::length(*p[0] - *p[1]);
        if (cd == d)
            return;
        glm::vec2 nd = glm::normalize(*p[0] - *p[1]) * (d - cd) * attri.bounce / cd;
        //glm::vec2 nd = glm::normalize(*p[0] - *p[1]) * (d - cd) * 0.5f * attri.bounce;
        *p[0] += nd;
        *p[1] -= nd;
    }

    void Stick::render(const glm::mat4& o) {
        renderer->draw(o, *p[0], *p[1], glm::vec3(1), attri.line_width);
        Circle::renderer->bind();
        Circle c = Circle(*p[0], attri.node_color, attri.node_size);
        Circle::renderer->render(o, c);
        c = Circle(*p[1], attri.node_color, attri.node_size);
        Circle::renderer->render(o, c);
        Circle::renderer->unbind();
    }

}

