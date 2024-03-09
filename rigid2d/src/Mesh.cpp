#include "Mesh.h"

#include "logger.h"
#include "Circle.h"
#include <cmath>

namespace mfw {

    Mesh::Mesh(Stick::Attribute attribute)
        : attri(attribute)
    {}

    Mesh::~Mesh()
    {
        for (auto& e : entities) {
            delete e;
        }
        for (auto& e : sticks) {
            delete e;
        }
    }

    void Mesh::update() {
        for (auto& stick : sticks) {
            stick->update();
        }
    }

    void Mesh::render(const glm::mat4& o) {
        for (auto & stick : sticks) {
            stick->render(o);
        }
    }

    void InitString(Mesh& mesh, i32 node, f32 d, const glm::vec2& pos)
    {
        ASSERT(node > 0);
        mesh.node = node;
        mesh.d = d;

        auto& entities = mesh.entities;
        auto& sticks = mesh.sticks;
        auto& attri = mesh.attri;

        entities.reserve(node + 1);
        sticks.reserve(node);

        for (f32 i = 0; i < entities.capacity(); i++) {

            entities.emplace_back(new Circle(glm::vec2(0, i * d) + pos, attri.node_color, attri.node_size));
        }
        for (i32 i = 0; i < node - 1; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[i + 1]->m_pos, d, attri));
        }
    }

    void InitBox(Mesh& mesh, f32 l, const glm::vec2& pos)
    {
        ASSERT(l > 0);
        mesh.node = 4;
        mesh.d = l;
        f32 lh = l * 0.5;

        auto& entities = mesh.entities;
        auto& sticks = mesh.sticks;
        auto& attri = mesh.attri;

        entities.reserve(4);
        sticks.reserve(6);

        entities.emplace_back(new Circle(glm::vec2(lh, lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(lh, -lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(-lh, -lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(-lh, lh) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(0) + pos, attri.node_color, attri.node_size));

        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[1]->m_pos, l, attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[2]->m_pos, l, attri));
        sticks.emplace_back(new Stick(&entities[2]->m_pos, &entities[3]->m_pos, l, attri));
        sticks.emplace_back(new Stick(&entities[3]->m_pos, &entities[0]->m_pos, l, attri));
        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[2]->m_pos, l * std::sqrt(2.f), attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[3]->m_pos, l * std::sqrt(2.f), attri));
        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[4]->m_pos, l * std::sqrt(2.f) * 0.5f, attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[4]->m_pos, l * std::sqrt(2.f) * 0.5f, attri));
        sticks.emplace_back(new Stick(&entities[2]->m_pos, &entities[4]->m_pos, l * std::sqrt(2.f) * 0.5f, attri));
        sticks.emplace_back(new Stick(&entities[3]->m_pos, &entities[4]->m_pos, l * std::sqrt(2.f) * 0.5f, attri));
    }


    void InitTriangle(Mesh& mesh, f32 l, const glm::vec2& pos) {
        mesh.node = 3;
        mesh.d = sqrt(3.0f) * l;

        auto& entities = mesh.entities;
        auto& sticks = mesh.sticks;
        auto& attri = mesh.attri;
        auto& d = mesh.d;

        entities.reserve(3);
        sticks.reserve(3);

        entities.emplace_back(new Circle(glm::vec2(0, l) + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(d, -l) * 0.5f + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(-d, -l) * 0.5f + pos, attri.node_color, attri.node_size));
        entities.emplace_back(new Circle(glm::vec2(0) + pos, attri.node_color, attri.node_size));

        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[1]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[2]->m_pos, d, attri));
        sticks.emplace_back(new Stick(&entities[2]->m_pos, &entities[0]->m_pos, d, attri));

        sticks.emplace_back(new Stick(&entities[0]->m_pos, &entities[3]->m_pos, l, attri));
        sticks.emplace_back(new Stick(&entities[1]->m_pos, &entities[3]->m_pos, l, attri));
        sticks.emplace_back(new Stick(&entities[2]->m_pos, &entities[3]->m_pos, l, attri));
    }

    void InitCircle(Mesh& mesh, f32 r, i32 n, const glm::vec2& pos) {
        static const f32 pi = 3.14159265359;
        mesh.node = 3;
        mesh.d = r;
        auto& entities = mesh.entities;
        auto& sticks = mesh.sticks;
        auto& attri = mesh.attri;

        f32 ri = 2 * pi / n;
        f32 nlen = glm::length(r * glm::vec2(sin(0) - sin(ri), cos(0) - cos(ri)));

        entities.reserve(n + 1);
        sticks.reserve(n * 3);

        for (i32 i = 0; i < n; i++) {
            f32 a = ri * i;
            entities.emplace_back(new Circle(r * glm::vec2(sin(a), cos(a)) + pos, attri.node_color, attri.node_size));
        }
        for (i32 i = 0; i < n - 1; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[i + 1]->m_pos, nlen, attri));
        }
        sticks.emplace_back(new Stick(&entities[n - 1]->m_pos, &entities[0]->m_pos, nlen, attri));

        entities.emplace_back(new Circle(glm::vec2(0) + pos, attri.node_color, attri.node_size));
        for (i32 i = 0; i < n; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[n]->m_pos, r, attri));
        }

        return;
        nlen = glm::length(r * glm::vec2(sin(0) - sin(ri * 2), cos(0) - cos(ri * 2)));
        for (i32 i = 0; i < n; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[(i + 2) % n]->m_pos, nlen, attri));
        }
        nlen = glm::length(r * glm::vec2(sin(0) - sin(ri * 3), cos(0) - cos(ri * 3)));
        for (i32 i = 0; i < n; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[(i + 3) % n]->m_pos, nlen, attri));
        }
        nlen = glm::length(r * glm::vec2(sin(0) - sin(ri * 4), cos(0) - cos(ri * 4)));
        for (i32 i = 0; i < n; i++) {
            sticks.emplace_back(new Stick(&entities[i]->m_pos, &entities[(i + 4) % n]->m_pos, nlen, attri));
        }
    }

}

