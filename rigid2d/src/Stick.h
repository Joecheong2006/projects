#pragma once

#include "Circle.h"

#define COLOR(val) ((u32)val >> 16) / 255.0, (((u32)val << 16) >> 24) / 255.0, (((u32)val << 24) >> 24) / 255.0

namespace mfw {
    class Stick {
        public:
        static struct Attribute {
            glm::vec4 node_color;
            f32 node_size;
            f32 hardness;
            f32 line_width;
            Attribute()
                : node_color(glm::vec4(COLOR(0x859584), 0)), node_size(0.4), hardness(0.9), line_width(0.12)
            {}
        } attribute;

        static class Renderer {
        private:
            VertexArray m_vao;
            ShaderProgram m_shader;
            VertexBuffer m_vbo;
            Texture2D m_texture;

        public:
            Renderer();

            void bind();
            void unbind();
            void render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w);
            void draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w);

        }* renderer;

        Stick(Circle* p1, Circle* p2, f32 d, const Attribute& attribute = Stick::attribute);
        void update();
        void render(const glm::mat4& o);
        void render_node(const glm::mat4& o);
        void render_line(const glm::mat4& o);
        void setAttribute(const Attribute& attri);

        f32 d;
        Circle* p[2];
        Attribute attri;

    };
}

