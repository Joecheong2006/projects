#pragma once

#include "Circle.h"

#define COLOR(val) ((u32)val >> 16) / 255.0, (((u32)val << 16) >> 24) / 255.0, (((u32)val << 24) >> 24) / 255.0

class Stick {
    public:
    struct Attribute {
        glm::vec4 node_color;
        f32 node_size;
        f32 hardness;
        f32 line_width;
    };

    static class Renderer {
    private:
        mfw::VertexArray m_vao;
        mfw::ShaderProgram m_shader;
        mfw::VertexBuffer m_vbo;
        mfw::Texture2D m_texture;

    public:
        Renderer();

        void bind();
        void unbind();
        void render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w);
        void render(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w);
        void draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec4 color, f32 w);
        void draw(const glm::mat4& o, const glm::vec2& p1, const glm::vec2& p2, glm::vec3 color, f32 w);

    }* renderer;

    Stick(Circle* p1, Circle* p2, f32 d, const Attribute& attribute);
    void update(const f64& dt);
    void render(const glm::mat4& o);
    void setAttribute(const Attribute& attri);

    f32 d;
    Circle* p[2];
    Attribute attri;

};

