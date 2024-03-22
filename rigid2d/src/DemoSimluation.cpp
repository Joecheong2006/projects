#include "DemoSimluation.h"

#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "VertexBufferLayout.h"
#include "World.h"
#include "Input.h"
#include "Application.h"
#include <list>

Simluation* Simluation::Instance = new DemoSimluation();

void wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
    static f64 bounce = 0.5;
    glm::vec2 a{}, s{};
    if (c->m_pos.y - c->r < -world.y) {
        s.y = -world.y - c->m_pos.y + c->r;
        a.y = (c->m_velocity.y * (-bounce  - 1)) / dt;
    }
    if (c->m_pos.x - c->r < -world.x) {
        s.x = -world.x - c->m_pos.x + c->r;
        a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
    }
    else if (c->m_pos.x + c->r > world.x) {
        s.x = world.x - c->m_pos.x - c->r;
        a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
    }
    c->m_pos += s;
    c->m_acceleration += a;
};

DemoSimluation::DemoSimluation()
    : Simluation("Double Pendulum")
{
    world = World(glm::vec2(30, 14) * unitScale);
    reset();
}

void DemoSimluation::reset() {
    world.clear();

    attri.node_color = glm::vec4(glm::vec4(COLOR(0x858AA6), 0));
    attri.node_size = 0.24 * unitScale;
    attri.line_width = 0.08 * unitScale;
    attri.hardness = 1;

    addDoublePendulum(30, 4);

    for (i32 i = 0; i < 4; i++) {
        addFixPointConstraint(world, glm::vec2(-4, 4) * unitScale, attri.node_size * 1.5);
    }
    for (i32 i = 0; i < 4; i++) {
        addHorizontalPointConstraint(world, glm::vec2(4, 4) * unitScale, attri.node_size * 1.5);
    }

    // InitializePointConstraint();
    // SetupRotateBox();
}

void DemoSimluation::update(const f64& dt) {
    (void)dt;
    world.update(dt);
    for (auto& obj : world.getObjects<Circle>()) {
        wall_collision(dt, static_cast<Circle*>(obj), world.size);
    }
}

void DemoSimluation::render(mfw::Renderer& renderer) {
    glm::mat4 proj = camera.getProjection();
    for (auto& obj : world.getConstraint<PointConstraint>()) {
        const PointConstraint* point = static_cast<PointConstraint*>(obj);
        point->render(proj, renderer);
    }

    for (auto& obj : world.getConstraint<DistanceConstraint>()) {
        const DistanceConstraint* dc = static_cast<DistanceConstraint*>(obj);
        dc->render(proj, renderer, attri.line_width);

        continue;
        const glm::dvec2 a = dc->target[0]->m_pos, b = dc->target[1]->m_pos;
        const glm::dvec2 m = mouseToWorldCoord();
        const f64 d = glm::dot(b - a, m - a) / glm::length(b - a);
        const glm::dvec2 p = glm::normalize(b - a) * glm::clamp(d, 0.0, glm::length(b - a));
        glm::dvec2 offset = glm::normalize(m - a - p) * (f64)(attri.node_size * 0.5);
        offset = {};
        renderer.renderRactangle(proj, m, a + p + offset,
                red + glm::vec3(glm::length(a + p + offset - m)), 0.03 * unitScale);
    }

    auto& circles = world.getObjects<Circle>();

    for (auto& obj : circles) {
        const Circle* circle = static_cast<Circle*>(obj);
        circle->render(proj, renderer);
    }

    {
        static const i32 max = 200;
        static const f32 scale = 0.05;
        static std::list<glm::vec2> positions_trace{};

        if ((i32)positions_trace.size() >= max) {
            positions_trace.pop_front();
        }
        positions_trace.push_back(circles.back()->m_pos);

        f32 i = 0;
        for (auto iter = positions_trace.begin(); iter != positions_trace.end();) {
            const glm::vec2 p1 = *iter;
            ++iter;
            if (iter == positions_trace.end())
                break;
            const glm::vec2 p2 = *iter;
            const glm::vec3 trace = glm::vec3(COLOR(0xc73e3e)), background = glm::vec3(COLOR(0x191919));
            const glm::vec3 color = (trace - background) * (i++ / positions_trace.size()) + background;
            const f32 t = scale * unitScale * ((i + 30.0f) / positions_trace.size());
            renderer.renderCircle(proj, { p1, color, t });
            renderer.renderCircle(proj, { p2, color, t });
            renderer.renderRactangle(proj, p1, p2, color, t);
        }
    }

    renderer.renderRactangle(proj, glm::vec2(world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec2(-world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec4(1), 0.2 * unitScale);

}

