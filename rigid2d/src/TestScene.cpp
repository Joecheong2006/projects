#include "TestScene.h"

#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "VertexBufferLayout.h"
#include "World.h"
#include "Input.h"
#include "Application.h"
#include <list>

TestScene::TestScene() {
    world = World(glm::vec2(30, 14) * unitScale);
    reset();
}

void TestScene::reset() {
    world.clear();
    SetDefaultStickAttribute();
    //SetupDoublePendulum(30, 4);
    InitializePointConstraint();
    SetupRotateBox();
}

void TestScene::update(const f64& dt) {
    (void)dt;
}

void TestScene::render(mfw::Renderer& renderer) {
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
        const glm::dvec2 m = mouseToWorldCoord(mfw::Application::Get()->GetWindow());
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

void TestScene::SetupDoublePendulum(f64 angle, f64 d) {
    f64 r = angle * 3.14 / 180;
    glm::dvec2 direction = glm::normalize(glm::dvec2(cos(r), sin(r))) * d * (f64)unitScale;

    auto p1 = world.addObject<Circle>(glm::vec2(), attri.node_color, attri.node_size);
    auto p2 = world.addObject<Circle>(direction, attri.node_color, attri.node_size);
    auto p3 = world.addObject<Circle>(direction * 2.0, attri.node_color, attri.node_size);
    world.addConstraint<DistanceConstraint>(p1, p2, d * unitScale);
    world.addConstraint<DistanceConstraint>(p2, p3, d * unitScale);
    AddFixPointConstraint(world, glm::vec2(), attri.node_size * 1.5)
        ->target = p1;
    p3->m_mass = 2;
    p3->r = p3->m_mass * unitScale * 0.2;
}

void TestScene::SetupRotateBox() {
    InitBox(world, glm::vec2(), 5 * unitScale);
    auto& boxCenter = world.getObjects<Circle>().back();
    auto& c1 = world.getObjects<Circle>()[0];
    auto& c2 = world.getObjects<Circle>()[2];
    AddFixPointConstraint(world, glm::vec2(), attri.node_size * 1.5)
        ->target = boxCenter;

    auto h1 = AddHorizontalPointConstraint(world, glm::vec2(9, 0) * unitScale, attri.node_size * 1.5);
    auto h2 = AddHorizontalPointConstraint(world, glm::vec2(-9, 0) * unitScale, attri.node_size * 1.5);
    auto p1 = world.addObject<Circle>(h1->self.m_pos, attri.node_color, attri.node_size);
    auto p2 = world.addObject<Circle>(h2->self.m_pos, attri.node_color, attri.node_size);
    f64 l = glm::length(p1->m_pos - c1->m_pos);
    world.addConstraint<DistanceConstraint>(c1, p1, l);
    world.addConstraint<DistanceConstraint>(c2, p2, l);
    h1->target = p1;
    h2->target = p2;
}

void TestScene::InitializePointConstraint() {
    for (i32 i = 0; i < 4; i++) {
        AddFixPointConstraint(world, glm::vec2(-4, 4) * unitScale, attri.node_size * 1.5);
    }
    for (i32 i = 0; i < 4; i++) {
        AddHorizontalPointConstraint(world, glm::vec2(4, 4) * unitScale, attri.node_size * 1.5);
    }
}

void TestScene::SetDefaultStickAttribute() {
    attri.node_color = glm::vec4(glm::vec4(COLOR(0x858AA6), 0));
    attri.node_size = 0.24 * unitScale;
    attri.line_width = 0.08 * unitScale;
    attri.hardness = 1;
}

