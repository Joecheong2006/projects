#include "Simulation.h"

#include "ObjectBuilder.h"

#include "Renderer.h"
#include "mfwlog.h"
#include "Input.h"
#include "Application.h"

Simulation::Simulation(const std::string& name, f32 worldScale)
    : name(name), unitScale(worldScale)
{
    attri.node_size *= unitScale;
    attri.line_width *= unitScale;
    attri.node_color = glm::vec3(COLOR(0x858AA6));
    initialize = [this]() {
        world = World(glm::vec2(10, 10));
        auto buildFixPoint = ObjectBuilder<FixPoint>{glm::vec3(COLOR(0x486577)), 0.2f };
        buildFixPoint({});
    };
}

void Simulation::update(const f64& dt) {
    world.update(dt);
}
void Simulation::render(mfw::Renderer& renderer) {
    world.render(camera.getProjection(), renderer);
}

glm::dvec2 Simulation::mouseToWorldCoord() {
    auto&& main = mfw::Application::Get().GetWindow();
    auto& mouse = mfw::Input::GetMouse();
    f32 width = main.width(), height = main.height();
    glm::vec4 uv = glm::vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0f - 1.0f;
    return camera.view * (uv * ((glm::vec4(1) / camera.ortho) / camera.scale));
}

void addString(const glm::vec2& pos, u32 node, f32 length) {
    ASSERT(node > 1);
    auto sim = Simulation::Get();
    auto& world = sim->world;
    const auto& attri = sim->attri;

    auto p1 = world.addRigidBody<Circle>(glm::vec2(0, 0) + pos, attri.node_color, attri.node_size);
    for (f32 i = 1; i < node; i++) {
        auto p2 = world.addRigidBody<Circle>(glm::vec2(0, -i * length) + pos, attri.node_color, attri.node_size);
        world.addConstraint<DistanceConstraint>(p1, p2, length, attri.line_width);
        p1 = p2;
    }
}

void addCircle(const glm::vec2& pos, i32 n, f32 r, i32 nstep) {
    ASSERT(r > 0 && n > 2 && nstep > 0);
    auto sim = Simulation::Get();
    static const f32 pi = 3.14159265359f;

    f32 ri = 2 * pi / n;
    f32 nlen = glm::length(r * glm::vec2(sin(0) - sin(ri), cos(0) - cos(ri)));

    auto& world = sim->world;
    const auto& attri = sim->attri;

    auto center = world.addRigidBody<Circle>(glm::vec2(0) + pos, attri.node_color, attri.node_size);

    RigidBody* p1 = world.addRigidBody<Circle>(r * glm::vec2(sin(0), cos(0)) + pos, attri.node_color, attri.node_size);
    RigidBody* first = p1;
    RigidBody* p2;

    for (i32 i = 1; i < n; i++) {
        f32 a = ri * i;
        p2 = world.addRigidBody<Circle>(r * glm::vec2(sin(a), cos(a)) + pos, attri.node_color, attri.node_size);
        world.addConstraint<DistanceConstraint>(p1, center, r, attri.line_width);
        world.addConstraint<DistanceConstraint>(p1, p2, nlen, attri.line_width);
        p1 = p2;
    }

    world.addConstraint<DistanceConstraint>(p1, first, nlen, attri.line_width);
    world.addConstraint<DistanceConstraint>(p1, center, r, attri.line_width);
}

void addBox(const glm::vec2& pos, f32 l)
{
    ASSERT(l > 0);
    addCircle(pos, 4, sqrt(2.0f) * l * 0.5);
}

void addTriangle(const glm::vec2& pos, f32 l) {
    ASSERT(l > -1);
    addCircle(pos, 3, l);
}

PointConstraint* addHorizontalPointConstraint(const glm::dvec2& pos) {
    auto result = Simulation::Get()->world.addConstraint<Roller>();
    result->m_pos = pos;
    return result;
}

PointConstraint* addFixPointConstraint(const glm::dvec2& pos) {
    auto result = Simulation::Get()->world.addConstraint<FixPoint>();
    result->m_pos = pos;
    return result;
}

void addDoublePendulum(f64 angle, f64 d) {
    auto sim = Simulation::Get();
    f64 r = angle * 3.14 / 180;
    auto& world = sim->world;
    const auto& attri = sim->attri;
    const f32 worldScale = sim->getWorldScale();
    glm::dvec2 direction = glm::normalize(glm::dvec2(cos(r), sin(r))) * d * (f64)worldScale;
    auto p1 = world.addRigidBody<Circle>(glm::vec2(), attri.node_color, attri.node_size * 1.5);
    auto p2 = world.addRigidBody<Circle>(direction, attri.node_color, attri.node_size * 1.5);
    auto p3 = world.addRigidBody<Circle>(direction * 2.0, attri.node_color, attri.node_size * 1.5);
    world.addConstraint<DistanceConstraint>(p1, p2, d * worldScale, attri.line_width);
    world.addConstraint<DistanceConstraint>(p2, p3, d * worldScale, attri.line_width);
    addFixPointConstraint(glm::vec2())
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
    const f32 worldScale = sim->getWorldScale();
    addBox(glm::vec2(), 3 * worldScale);

    auto& circles = world.getObjects<Circle>();
    i32 len = circles.size();
    auto boxCenter = circles[len - 5];
    auto c1 = circles[len - 2];
    auto c2 = circles[len - 4];
    addFixPointConstraint(glm::vec2())
        ->target = boxCenter;

    auto h1 = addHorizontalPointConstraint(glm::vec2(6, 0) * worldScale);
    auto h2 = addHorizontalPointConstraint(glm::vec2(-6, 0) * worldScale);
    auto p1 = world.addRigidBody<Circle>(h1->m_pos, attri.node_color, attri.node_size);
    auto p2 = world.addRigidBody<Circle>(h2->m_pos, attri.node_color, attri.node_size);
    f64 l = glm::length(p1->m_pos - c1->m_pos);
    world.addConstraint<DistanceConstraint>(c1, p1, l, attri.line_width);
    world.addConstraint<DistanceConstraint>(c2, p2, l, attri.line_width);
    h1->target = p1;
    h2->target = p2;
}

