#pragma once

#include "Object.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Collision.h"

namespace mfw {
    class Circle : public Object2D, public Collision::Colliable<Circle> {
    public:
        static class Renderer {
        private:
            VertexArray m_vao;
            VertexBuffer m_vbo;
            ShaderProgram m_shader;
            Texture2D m_texture;

        public:
            Renderer();

            void bind();
            void unbind();
            void render(const glm::mat4& o, Circle& circle);
            void draw(const glm::mat4& o, Circle& circle);

        }* renderer;

        f32 r;
        Circle();
        Circle(const glm::vec2& pos, const glm::vec4& color, const f32& d);

        virtual bool collide(Circle& obj) override;
        virtual void solveCollision(Circle& obj) override;

    };
}

