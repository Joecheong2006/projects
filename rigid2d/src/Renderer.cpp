#include "Renderer.h"
#include "glad/gl.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <mfw/VertexBufferLayout.h>
#include "Circle.h"

static f32 vertexs[] = {
    -1.0f, -1.0f, 0.0, 0.0,
    1.0f, -1.0f,  1.0, 0.0,
    -1.0f, 1.0f,  0.0, 1.0,

    1.0f, 1.0f,   1.0, 1.0,
    1.0f, -1.0f,  1.0, 0.0,
    -1.0f, 1.0f,  0.0, 1.0,
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

    ImageRenderer::ImageRenderer(const char* path, i32 wrap, i32 filter)
        : m_vbo(vertexs, sizeof(vertexs), GL_STATIC_DRAW), m_texture(path, wrap, filter, filter)
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

    ImageRenderer *iCircleRenderer, *iLineRenderer, *iRingRenderer;

    class ShaderRenderer {
    public:
        VertexArray m_vao;
        ShaderProgram m_shader;
        VertexBuffer m_vbo;
        ShaderRenderer(const char* vert, const char* frag)
            : m_vbo(vertexs, sizeof(vertexs), GL_STATIC_DRAW)
        {
            VertexBufferLayout cube_layout;
            cube_layout.add<f32>(2);
            cube_layout.add<f32>(2);
            m_vao.applyBufferLayout(cube_layout);

            m_shader.attachShader(GL_VERTEX_SHADER, vert);
            m_shader.attachShader(GL_FRAGMENT_SHADER, frag);
            m_shader.link();

            m_vao.unbind();
            m_vbo.unbind();
            m_shader.unbind();
        }

    }* renderer, *crenderer, *ringRenderer;

    Renderer::Renderer() {
        iCircleRenderer = new ImageRenderer("res/images/circle.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
        iLineRenderer = new ImageRenderer("res/images/square.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
        iRingRenderer = new ImageRenderer("res/images/ring.png", GL_CLAMP_TO_EDGE, GL_LINEAR);
        renderer = new ShaderRenderer("res/shaders/square.vert", "res/shaders/square.frag");
        crenderer = new ShaderRenderer("res/shaders/circle.vert", "res/shaders/circle.frag");
        ringRenderer = new ShaderRenderer("res/shaders/ring.vert", "res/shaders/ring.frag");
    }

    Renderer::~Renderer() {
        delete iCircleRenderer;
        delete iLineRenderer;
        delete iRingRenderer;
        delete renderer;
        delete crenderer;
        delete ringRenderer;
    }

    void Renderer::clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::renderLine(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        renderLine(proj, p1, p2, glm::vec4(color, 1), w);
    }

    void Renderer::renderLine(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3((p1 + p2) * 0.5f, 0));
        view = glm::rotate(view, glm::atan((p1.y - p2.y) / (p1.x - p2.x)), glm::vec3(0, 0, 1));
        view = glm::scale(view, glm::vec3(glm::length(p1 - p2) * 0.5, w, 1));

        renderer->m_shader.bind();
        renderer->m_vao.bind();
        renderer->m_shader.set4f("color", color);
        renderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderLineI(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w) {
        renderLineI(proj, p1, p2, glm::vec4(color, 1), w);
    }

    void Renderer::renderLineI(const glm::mat4& proj, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3((p1 + p2) * 0.5f, 0));
        view = glm::rotate(view, 3.14f * 0.5f, glm::vec3(0, 0, -1));
        view = glm::rotate(view, glm::atan((p1.y - p2.y) / (p1.x - p2.x)), glm::vec3(0, 0, 1));
        view = glm::scale(view, glm::vec3(w, glm::length(p1 - p2) * 0.5, 1));

        iLineRenderer->m_texture.bind();
        iLineRenderer->m_shader.bind();
        iLineRenderer->m_vao.bind();
        iLineRenderer->m_shader.set1i("tex", 0);
        iLineRenderer->m_shader.set4f("color", color);
        iLineRenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircle(const glm::mat4& proj, const Circle& circle) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle.m_position.x, circle.m_position.y, 0));
        view = glm::scale(view, glm::vec3(circle.radius, circle.radius, 1));

        crenderer->m_shader.bind();
        crenderer->m_vao.bind();
        crenderer->m_shader.set4f("color", glm::vec4(circle.m_color, 1));
        crenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircle(const glm::mat4& proj, const Circle* circle) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle->m_position.x, circle->m_position.y, 0));
        view = glm::scale(view, glm::vec3(circle->radius, circle->radius, 0));

        crenderer->m_shader.bind();
        crenderer->m_vao.bind();
        crenderer->m_shader.set4f("color", glm::vec4(circle->m_color, 1));
        crenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircle(const glm::mat4& proj, const glm::vec2& p, f32 radius, glm::vec4 color) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(p.x, p.y, 0));
        view = glm::scale(view, glm::vec3(radius, radius, 0));

        crenderer->m_shader.bind();
        crenderer->m_vao.bind();
        crenderer->m_shader.set4f("color", color);
        crenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircleI(const glm::mat4& proj, const Circle& circle) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle.m_position.x, circle.m_position.y, 0));
        view = glm::scale(view, glm::vec3(circle.radius, circle.radius, 1));

        iCircleRenderer->m_texture.bind();
        iCircleRenderer->m_shader.bind();
        iCircleRenderer->m_vao.bind();
        iCircleRenderer->m_shader.set1i("tex", 0);
        iCircleRenderer->m_shader.set4f("color", glm::vec4(circle.m_color, 1));
        iCircleRenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircleI(const glm::mat4& proj, const Circle* circle) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(circle->m_position.x, circle->m_position.y, 0));
        view = glm::scale(view, glm::vec3(circle->radius, circle->radius, 0));

        iCircleRenderer->m_texture.bind();
        iCircleRenderer->m_shader.bind();
        iCircleRenderer->m_vao.bind();
        iCircleRenderer->m_shader.set1i("tex", 0);
        iCircleRenderer->m_shader.set4f("color", glm::vec4(circle->m_color, 1));
        iCircleRenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderCircleI(const glm::mat4& proj, const glm::vec2& p, f32 radius, glm::vec4 color) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(p.x, p.y, 0));
        view = glm::scale(view, glm::vec3(radius, radius, 0));

        iCircleRenderer->m_texture.bind();
        iCircleRenderer->m_shader.bind();
        iCircleRenderer->m_vao.bind();
        iCircleRenderer->m_shader.set1i("tex", 0);
        iCircleRenderer->m_shader.set4f("color", color);
        iCircleRenderer->m_shader.setMat4("view", proj * view);

        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderRing(const glm::mat4& proj, const glm::vec2& p, f32 radius, f32 width, glm::vec4 color) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(p.x, p.y, 0));
        view = glm::scale(view, glm::vec3(radius, radius, 0));

        ringRenderer->m_shader.bind();
        ringRenderer->m_vao.bind();
        ringRenderer->m_shader.set1f("width", width);
        ringRenderer->m_shader.set4f("color", color);
        ringRenderer->m_shader.setMat4("view", proj * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

    void Renderer::renderRingI(const glm::mat4& proj, const glm::vec2& p, f32 radius, glm::vec4 color) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(p.x, p.y, 0));
        view = glm::scale(view, glm::vec3(radius, radius, 0));

        iRingRenderer->m_texture.bind();
        iRingRenderer->m_shader.bind();
        iRingRenderer->m_vao.bind();
        iRingRenderer->m_shader.set1i("tex", 0);
        iRingRenderer->m_shader.set4f("color", color);
        iRingRenderer->m_shader.setMat4("view", proj * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }

}
