#include "Renderer.h"
#include "glad/gl.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "ShaderProgram.h"
#include "Texture2D.h"

#include "Circle.h"

static f32 vertexs[] = {
    -1.0f, -1.0f, 0.0, 0.0,
    1.0f, -1.0f,  1.0, 0.0,
    -1.0f, 1.0f,  0.0, 1.0,

    1.0f, 1.0f,   1.0, 1.0,
    -1.0f, 1.0f,  1.0, 0.0,
    1.0f, -1.0f,  0.0, 1.0,
};

namespace mfw {
    void GLClearError()
    {
        while(glGetError() != GL_NO_ERROR);
    }

    bool GLLogCall(const char* file, i32 line, const char* func)
    {
        while(GLenum error = glGetError())
        {
            LOG_INFO("[{}:{}:{}:{}]", file, line, func, error);
            return false;
        }
        return true;
    }

    ImageRenderer::ImageRenderer(const char* path)
        : m_vbo(vertexs, sizeof(vertexs)), m_texture(path)
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

    ImageRenderer *ractangleRenderer, *circleRenderer;

    Renderer::Renderer() {
        circleRenderer = new ImageRenderer("res/images/circle.png");
        ractangleRenderer = new ImageRenderer("res/images/square.png");
    }

    Renderer::~Renderer() {
        delete circleRenderer;
        delete ractangleRenderer;
    }

    void Renderer::clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::renderRactangle(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        ractangleRenderer->m_texture.bind();
        ractangleRenderer->m_shader.bind();
        ractangleRenderer->m_vao.bind();
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3((p1 + p2) * 0.5f, 0));
        view = glm::rotate(view, glm::atan((p1.y - p2.y) / (p1.x - p2.x)), glm::vec3(0, 0, 1));
        view = glm::scale(view, glm::vec3(glm::length(p1 - p2) * 0.5, w, 0));
        ractangleRenderer->m_shader.set1i("tex", 0);
        ractangleRenderer->m_shader.set3f("color", color);
        ractangleRenderer->m_shader.setMat4("view", proj * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircle(const glm::mat4& proj, const Circle& circle) {
        circleRenderer->m_texture.bind();
        circleRenderer->m_shader.bind();
        circleRenderer->m_vao.bind();
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle.m_pos.x, circle.m_pos.y, 0));
        view = glm::scale(view, glm::vec3(circle.r, circle.r, 1));
        circleRenderer->m_shader.set1i("tex", 0);
        circleRenderer->m_shader.set3f("color", circle.m_color);
        circleRenderer->m_shader.setMat4("view", proj * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircle(const glm::mat4& proj, const Circle* circle) {
        circleRenderer->m_texture.bind();
        circleRenderer->m_shader.bind();
        circleRenderer->m_vao.bind();
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle->m_pos.x, circle->m_pos.y, 0));
        view = glm::scale(view, glm::vec3(circle->r, circle->r, 1));
        circleRenderer->m_shader.set1i("tex", 0);
        circleRenderer->m_shader.set3f("color", circle->m_color);
        circleRenderer->m_shader.setMat4("view", proj * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

}
