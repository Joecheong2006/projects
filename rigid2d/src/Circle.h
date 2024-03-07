#ifndef _CIRCLE_H
#define _CIRCLE_H

#include "Object.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "Collision.h"

namespace mfw {
    class Circle : public Object2D, public Collision::Colliable<Circle> {
    public:
        static class Renderer {
        private:
            VertexArray m_vao;
            VertexBuffer m_vbo;
            ShaderProgram m_shader;

        public:
            Renderer();

            inline void bind() {
                m_vao.bind();
                m_shader.bind();
            }

            inline void unbind() {
                m_shader.unbind();
                m_vao.unbind();
            }

            void render(const glm::mat4& o, Circle& circle);
            void draw(const glm::mat4& o, Circle& circle);

        }* renderer;

        f32 d;
        Circle();
        Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d);

        virtual bool collide(Circle& obj) override;
        virtual void solveCollision(Circle& obj) override;

    };
}

#endif
