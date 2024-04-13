#include "Simulation.h"

#include "ObjectBuilder.h"

#include "Renderer.h"
#include <mfw/mfwlog.h>
#include <mfw/Application.h>
#include <mfw/Input.h>

Simulation::Simulation(const std::string& name, real worldScale)
    : name(name), unitScale(worldScale)
{
    attri.node_size *= unitScale;
    attri.line_width *= unitScale;
    attri.node_color = color(COLOR(0x858AA6));
    initialize = [this]() {
        world.initialize();
        auto buildFixPoint = ObjectBuilder<FixPoint>{color(COLOR(0x486577)), 0.2f };
        buildFixPoint({});
    };
}

void Simulation::update(const real& dt) {
    world.update(dt);
}
void Simulation::render(mfw::Renderer& renderer) {
    world.render(camera.getProjection(), renderer);
}

vec2 Simulation::mouseToWorldCoord() {
    auto&& main = mfw::Application::Get().GetWindow();
    auto& mouse = mfw::Input::GetMouse();
    real width = main.width(), height = main.height();
    vec4 uv = vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0 - 1.0;
    return camera.view * (uv * ((vec4(1.0) / camera.ortho) / camera.scale));
}

void addString(const vec2& pos, u32 node, real length) {
    ASSERT(node > 1);
    auto sim = Simulation::Get();
    auto& world = sim->world;
    const auto& attri = sim->attri;

    auto p1 = world.addRigidBody<Circle>(vec2(0, 0) + pos, attri.node_color, attri.node_size);
    for (real i = 1; i < node; i++) {
        auto p2 = world.addRigidBody<Circle>(vec2(0, -i * length) + pos, attri.node_color, attri.node_size);
        world.addConstraint<DistanceConstraint>(p1, p2, length, attri.line_width);
        p1 = p2;
    }
}

void addCircle(const vec2& pos, i32 n, real r, i32 nstep) {
    ASSERT(r > 0 && n > 2 && nstep > 0);
    auto sim = Simulation::Get();
    static const real pi = 3.14159265359f;

    real ri = 2 * pi / n;
    real nlen = glm::length(r * vec2(sin(0) - sin(ri), cos(0) - cos(ri)));

    auto& world = sim->world;
    const auto& attri = sim->attri;

    auto center = world.addRigidBody<Circle>(vec2(0) + pos, attri.node_color, attri.node_size);

    RigidBody* p1 = world.addRigidBody<Circle>(r * vec2(sin(0), cos(0)) + pos, attri.node_color, attri.node_size);
    RigidBody* first = p1;
    RigidBody* p2;

    for (i32 i = 1; i < n; i++) {
        real a = ri * i;
        p2 = world.addRigidBody<Circle>(r * vec2(sin(a), cos(a)) + pos, attri.node_color, attri.node_size);
        world.addConstraint<DistanceConstraint>(p1, center, r, attri.line_width);
        world.addConstraint<DistanceConstraint>(p1, p2, nlen, attri.line_width);
        p1 = p2;
    }

    world.addConstraint<DistanceConstraint>(p1, first, nlen, attri.line_width);
    world.addConstraint<DistanceConstraint>(p1, center, r, attri.line_width);
}

void addBox(const vec2& pos, real l)
{
    ASSERT(l > 0);
    addCircle(pos, 4, sqrt(2.0f) * l * 0.5);
}

void addTriangle(const vec2& pos, real l) {
    ASSERT(l > -1);
    addCircle(pos, 3, l);
}

PointConstraint* addHorizontalPointConstraint(const vec2& pos) {
    auto result = Simulation::Get()->world.addConstraint<Roller>();
    result->m_position = pos;
    return result;
}

PointConstraint* addFixPointConstraint(const vec2& pos) {
    auto result = Simulation::Get()->world.addConstraint<FixPoint>();
    result->m_position = pos;
    return result;
}

void addDoublePendulum(real angle, real d) {
    auto sim = Simulation::Get();
    real r = angle * 3.14 / 180;
    auto& world = sim->world;
    const auto& attri = sim->attri;
    const real worldScale = sim->getWorldScale();
    vec2 direction = glm::normalize(vec2(cos(r), sin(r))) * d * (real)worldScale;
    auto p1 = world.addRigidBody<Circle>(vec2(), attri.node_color, attri.node_size * 1.5);
    auto p2 = world.addRigidBody<Circle>(direction, attri.node_color, attri.node_size * 1.5);
    auto p3 = world.addRigidBody<Circle>(direction * 2.0, attri.node_color, attri.node_size * 1.5);
    world.addConstraint<DistanceConstraint>(p1, p2, d * worldScale, attri.line_width);
    world.addConstraint<DistanceConstraint>(p2, p3, d * worldScale, attri.line_width);
    addFixPointConstraint(vec2())
        ->target = p1;
    // p3->m_mass = p1->m_mass * 1;
    p1->drawEnable = false;
    p2->drawEnable = false;
    p3->drawEnable = false;

    auto tracer = world.addConstraint<Tracer>(p3);
    tracer->maxScale = 0.1 * worldScale;
    tracer->minScale = 0.01 * worldScale;
    tracer->maxSamples = 200;
    tracer->dr = 0.7;
}

void SetupRotateBox() {
    auto sim = Simulation::Get();
    auto& world = sim->world;
    const auto& attri = sim->attri;
    const real worldScale = sim->getWorldScale();
    addBox(vec2(), 3 * worldScale);

    auto& circles = world.getObjects<Circle>();
    i32 len = circles.size();
    auto boxCenter = circles[len - 5];
    auto c1 = circles[len - 2];
    auto c2 = circles[len - 4];
    addFixPointConstraint(vec2())
        ->target = boxCenter;

    auto h1 = addHorizontalPointConstraint(vec2(6, 0) * worldScale);
    auto h2 = addHorizontalPointConstraint(vec2(-6, 0) * worldScale);
    auto p1 = world.addRigidBody<Circle>(h1->m_position, attri.node_color, attri.node_size);
    auto p2 = world.addRigidBody<Circle>(h2->m_position, attri.node_color, attri.node_size);
    real l = glm::length(p1->m_position - c1->m_position);
    world.addConstraint<DistanceConstraint>(c1, p1, l, attri.line_width);
    world.addConstraint<DistanceConstraint>(c2, p2, l, attri.line_width);
    h1->target = p1;
    h2->target = p2;
}

