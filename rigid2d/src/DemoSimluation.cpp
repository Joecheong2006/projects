#include "DemoSimluation.h"

#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "VertexBufferLayout.h"
#include "World.h"
#include "Input.h"
#include "Application.h"

void wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
    static f64 bounce = 0.1;
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
    addTracer(world, world.getObjects<Circle>().back());
    return;

    for (i32 i = 0; i < 4; i++) {
        addFixPointConstraint(world, glm::vec2(-4, 4) * unitScale, attri.node_size * 1.5);
    }
    for (i32 i = 0; i < 4; i++) {
        addHorizontalPointConstraint(world, glm::vec2(4, 4) * unitScale, attri.node_size * 1.5);
    }
    SetupRotateBox();
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
        obj->render(proj, renderer);
    }

    // auto& object = world.getConstraint<PointConstraint>();
    // for (i32 i = 0; i < (i32)object.size() - 1; ++i) {
    //     object[i]->render(proj, renderer);
    // }

    for (auto& obj : world.getConstraint<DistanceConstraint>()) {
        obj->render(proj, renderer);

        continue;
        DistanceConstraint* dc = static_cast<DistanceConstraint*>(obj);
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
        obj->render(proj, renderer);
    }

    // world.getConstraint<PointConstraint>().back()->render(proj, renderer);

    renderer.renderRactangle(proj, glm::vec2(world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec2(-world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec4(1), 0.2 * unitScale);

}

