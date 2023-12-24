#ifndef _CIRCLE_H
#define _CIRCLE_H

#include "Object.h"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "ShaderProgram.hpp"
#include "Texture2D.hpp"
#include <vector>
#include "Collision.h"

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
        std::vector<Circle> entities;
        Manager();
        Circle& create_circle(const glm::vec2& pos, const glm::vec3& color, const f32& d);
    };

    f32 d;
    Circle(const glm::vec2& pos, const glm::vec3& color, const f32& d);

    bool collide(Circle& obj);
    void solve_collision(Circle& obj);

};

#endif
