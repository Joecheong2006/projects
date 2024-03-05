#ifndef _CIRCLE_H
#define _CIRCLE_H

#include "Object.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Collision.h"

namespace mfw {
    class Circle : public Object2D, public Collision::Colliable<Circle> {
    public:
        class Manager {
            friend class Renderer;
        private:
            VertexArray m_vao;
            VertexBuffer m_vbo;
            ShaderProgram m_shader;
            Texture2D m_texture;
        public:
            Manager();
            Circle& createCircle(const glm::vec2& pos, const glm::vec3& color, const f32& d);
            Circle& createCircle(const f32& d);
            void renderCircle(const glm::mat4& o);
            std::vector<Circle> entities;

        };

        f32 d;
        Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d);

        virtual bool collide(Circle& obj) override;
        virtual void solveCollision(Circle& obj) override;

    };
}

#endif
