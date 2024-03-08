#pragma once

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"

#define COLOR(val) ((u32)val >> 16) / 255.0, (((u32)val << 16) >> 24) / 255.0, (((u32)val << 24) >> 24) / 255.0

namespace mfw {
    class Stick {
        public:
        static struct Attribute {
            glm::vec3 node_color;
            f32 node_size;
            f32 bounce;
            f32 line_width;
            Attribute()
                : node_color(glm::vec3(COLOR(0x859584))), node_size(0.3), bounce(1.0), line_width(0.08)
            {}
        } attribute;

        static class Renderer {
        private:
            VertexArray m_vao;
            ShaderProgram m_shader;
            VertexBuffer m_vbo;

        public:
            Renderer();

            inline void bind() {
                m_vao.bind();
                m_shader.bind();
            }

            inline void unbind() {
                m_vao.unbind();
                m_shader.unbind();
            }

            void render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w);
            void draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w);

        }* renderer;

        Stick(glm::vec2* p1, glm::vec2* p2, f32 d, Attribute& attribute = Stick::attribute);
        void update();
        void render(const glm::mat4& o);
        void render_node(const glm::mat4& o);
        void render_line(const glm::mat4& o);

        f32 d;
        glm::vec2* p[2];
        Attribute& attri;

    };
}

