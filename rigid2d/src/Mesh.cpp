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
        return;
        Stick::renderer->bind();
        for (auto & stick : sticks) {
            stick->render_line(o);
        }
        Stick::renderer->unbind();
        Circle::renderer->bind();
        for (auto & stick : sticks) {
            stick->render_node(o);
        }
        Circle::renderer->unbind();
    }

    void InitString(Mesh& mesh, const glm::vec2& pos, i32 node, f32 d)
    {
        ASSERT(node > 0);
        mesh.node = node;

        auto& entities = mesh.entities;
        auto& sticks = mesh.sticks;
        auto& attri = mesh.attri;

        entities.reserve(node + 1);
        sticks.reserve(node);

        for (f32 i = 0; i < entities.capacity(); i++) {

            entities.emplace_back(new Circle(glm::vec2(0, i * d) + pos, attri.node_color, attri.node_size));
        }
        for (i32 i = 0; i < node - 1; i++) {
            sticks.emplace_back(new Stick(entities[i], entities[i + 1], d, attri));
        }
    }

    void InitBox(Mesh& mesh, const glm::vec2& pos, f32 l)
    {
        ASSERT(l > 0);
        InitCircle(mesh, pos, sqrt(2.0f) * l * 0.5, 4);
    }


    void InitTriangle(Mesh& mesh, const glm::vec2& pos, f32 l) {
        InitCircle(mesh, pos, l, 3);
    }

    void InitCircle(Mesh& mesh, const glm::vec2& pos, f32 r, i32 n, i32 nstep) {
        ASSERT(r > 0 && n > 2 && nstep > 0);
        static const f32 pi = 3.14159265359;
        mesh.node = n;
        auto& entities = mesh.entities;
        auto& sticks = mesh.sticks;
        auto& attri = mesh.attri;

        f32 ri = 2 * pi / n;

        entities.reserve(n + 1);
        sticks.reserve(n * 3);

        for (i32 i = 0; i < n; i++) {
            f32 a = ri * i;
            entities.emplace_back(new Circle(r * glm::vec2(sin(a), cos(a)) + pos, attri.node_color, attri.node_size));
        }
        entities.emplace_back(new Circle(glm::vec2(0) + pos, attri.node_color, attri.node_size));

        for (i32 i = 0; i < n; i++) {
            sticks.emplace_back(new Stick(entities[i], entities[n], r, attri));
        }

        for (i32 step = 1; step <= nstep; step++) {
            f32 nlen = glm::length(r * glm::vec2(sin(0) - sin(ri * step), cos(0) - cos(ri * step)));
            for (i32 i = 0; i < n; i++) {
                sticks.emplace_back(new Stick(entities[i], entities[(i + step) % n], nlen, attri));
            }
        }
    }

}

