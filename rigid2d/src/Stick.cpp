#include "Stick.h"
#include "Renderer.h"
#include "Circle.h"
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
    Stick::Renderer* Stick::renderer = nullptr;

    Stick::Renderer::Renderer()
        : m_vbo(vertexs, sizeof(vertexs)), m_texture("res/images/square.png")
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

    void Stick::Renderer::bind() {
        m_texture.bind();
        m_shader.bind();
        m_vao.bind();
    }

    void Stick::Renderer::unbind() {
        m_texture.unbind();
        m_shader.unbind();
        m_vao.unbind();
    }

    void Stick::Renderer::render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3((p1 + p2) * 0.5f, 0));
        view = glm::rotate(view, glm::atan((p1.y - p2.y) / (p1.x - p2.x)), glm::vec3(0, 0, 1));
        view = glm::scale(view, glm::vec3(glm::length(p1 - p2) * 0.5, w, 0));
        m_shader.set1i("tex", 0);
        m_shader.set4f("color", color);
        m_shader.setMat4("view", o * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Stick::Renderer::render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        glm::vec4 c4 = glm::vec4(color, 1);
        render(o, p1, p2, c4, w);
    }

    void Stick::Renderer::draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w) {
        bind();
        render(o, p1, p2, color, w);
        unbind();
    }

    void Stick::Renderer::draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        bind();
        render(o, p1, p2, color, w);
        unbind();
    }

    Stick::Stick(Circle* p1, Circle* p2, f32 d, const Attribute& attribute)
        : d(d), attri(attribute)
    {
        p[0] = p1;
        p[1] = p2;
    }

    void Stick::update(const f64& dt) {
        f64 cd = glm::length(p[0]->m_pos - p[1]->m_pos);
        glm::dvec2 nd = glm::normalize(p[0]->m_pos - p[1]->m_pos) * (d - cd) * 0.5;
        p[0]->m_pos += nd;
        p[1]->m_pos -= nd;
    }

    void Stick::render(const glm::mat4& o) {
        renderer->draw(o, p[0]->m_pos, p[1]->m_pos, glm::vec4(1, 1, 1, 0), attri.line_width);
        Circle::renderer->bind();
        Circle::renderer->render(o, *p[0]);
        Circle::renderer->render(o, *p[1]);
        Circle::renderer->unbind();
    }

    void Stick::render_node(const glm::mat4& o) {
        Circle::renderer->render(o, *p[0]);
        Circle::renderer->render(o, *p[1]);
    }

    void Stick::render_line(const glm::mat4& o) {
        renderer->draw(o, p[0]->m_pos, p[1]->m_pos, glm::vec4(1, 1, 1, 0), attri.line_width);
    }

    void Stick::setAttribute(const Attribute& attri) {
        this->attri = attri;
        p[0]->m_color = attri.node_color;
        p[0]->r = attri.node_size;
        p[1]->m_color = attri.node_color;
        p[1]->r = attri.node_size;
    }
}

