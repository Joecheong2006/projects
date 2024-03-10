#pragma once

#include "Stick.h"

namespace mfw {
    class Circle;
    struct Mesh {
        Stick::Attribute attri;
        Mesh(Stick::Attribute attribute = Stick::attribute);
        ~Mesh();

        void update();
        void render(const glm::mat4& o);

        std::vector<Circle*> entities;
        std::vector<Stick*> sticks;
        i32 node;

    };

    void InitString(Mesh& mesh, const glm::vec2& pos, i32 node, f32 d);
    void InitBox(Mesh& mesh, const glm::vec2& pos, f32 l);
    void InitTriangle(Mesh& mesh, const glm::vec2& pos, f32 l);
    void InitCircle(Mesh& mesh, const glm::vec2& pos, f32 r, i32 n, i32 nstep = 1);

}

