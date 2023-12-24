#include "Renderer.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        printf("[%d:%s %d %s]\n", error, file, line, function);
        return false;
    }
    return true;
}

void Renderer::draw(const VertexArray& vao, const IndexBuffer& ibo, const ShaderProgram& shader) const
{
    shader.bind();
    vao.bind();
    GLCALL(glDrawElements(GL_TRIANGLES, ibo.count(), GL_UNSIGNED_INT, NULL));
}

void Renderer::set_background_color(const glm::vec3& color) {
    glClearColor(color.r, color.g, color.b, 1);
}

void Renderer::clear()
{
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}

//static glm::mat4 o = glm::ortho(-32.0f, 32.0f, -48.0f, 48.0f, -1.0f, 1.0f);

void Renderer::draw_circle(Circle::Manager& manager)
{
    manager.m_texture.bind();
    manager.m_shader.bind();
    manager.m_shader.set_1i("tex", 0);
    manager.m_vao.bind();
    for(auto& c : manager.entities) {
        glm::mat4 view = glm::mat4(1);
        view = glm::translate(view, glm::vec3(c.m_pos.x, c.m_pos.y, 0));
        view = glm::scale(view, glm::vec3(c.d, c.d, 0));
        manager.m_shader.set_3f("color", c.m_color);
        manager.m_shader.set_m4("view", o * view);
        GLCALL(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    manager.m_texture.unbind();
    manager.m_shader.unbind();
    manager.m_vao.unbind();
}

