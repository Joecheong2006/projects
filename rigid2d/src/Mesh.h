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
        f32 d;

    };

    void InitString(Mesh& mesh, i32 node, f32 d, const glm::vec2& pos = glm::vec2(0));
    void InitBox(Mesh& mesh, f32 l, const glm::vec2& pos = glm::vec2(0));
    void InitTriangle(Mesh& mesh, f32 l, const glm::vec2& pos = glm::vec2(0));
    void InitCircle(Mesh& mesh, f32 r, i32 n, const glm::vec2& pos = glm::vec2(0));

}

