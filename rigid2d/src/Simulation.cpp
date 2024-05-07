#include "Simulation.h"

#include <mfw/mfwlog.h>
#include <mfw/Application.h>
#include <mfw/Input.h>

Simulation* Simulation::Instance = nullptr;

Simulation::Simulation(const std::string& name, real worldScale)
    : name(name), unitScale(worldScale)
{}

vec2 Simulation::mouseToWorldCoord() {
    auto&& main = mfw::Application::Get().GetWindow();
    auto& mouse = mfw::Input::GetMouse();
    real width = main.width(), height = main.height();
    vec4 uv = vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0 - 1.0;
    return camera.view * (uv * ((glm::dvec4(1.0) / camera.ortho) / camera.scale));
}

void addString(const vec2& pos, u32 node, real length) {
    ASSERT(node > 1);

    auto& world = Simulation::Get()->world;
    auto p1 = world.addRigidBody<Circle>(pos);
    for (real i = 1; i < node; i++) {
        auto p2 = world.addRigidBody<Circle>(vec2(0, -i * length) + pos);
        world.addConstraint<DistanceConstraint>(p1, p2, length);
        p1 = p2;
    }
}

void addCircle(const vec2& pos, i32 n, real r, i32 nstep) {
    ASSERT(r > 0 && n > 2 && nstep > 0);
    static const real pi = 3.14159265359f;
    auto& world = Simulation::Get()->world;

    real ri = 2 * pi / n;
    real nlen = glm::length(r * vec2(sin(0) - sin(ri), cos(0) - cos(ri)));

    auto center = world.addRigidBody<Circle>(pos);

    RigidBody* p1 = world.addRigidBody<Circle>(r * vec2(sin(0), cos(0)) + pos);
    RigidBody* first = p1;
    RigidBody* p2;

    for (i32 i = 1; i < n; i++) {
        real a = ri * i;
        p2 = world.addRigidBody<Circle>(r * vec2(sin(a), cos(a)) + pos);
        world.addConstraint<DistanceConstraint>(p1, center, r);
        world.addConstraint<DistanceConstraint>(p1, p2, nlen);
        p1 = p2;
    }

    world.addConstraint<DistanceConstraint>(p1, center, r);
    world.addConstraint<DistanceConstraint>(p1, first, nlen);
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
    auto& world = Simulation::Get()->world;
    real r = angle * 3.14 / 180;
    vec2 direction = glm::normalize(vec2(cos(r), sin(r))) * d;

    auto p1 = world.addRigidBody<Circle>(vec2{});
    auto p2 = world.addRigidBody<Circle>(direction);
    auto p3 = world.addRigidBody<Circle>(direction * 2.0);
    world.addConstraint<DistanceConstraint>(p1, p2, d);
    world.addConstraint<DistanceConstraint>(p2, p3, d);
    world.addConstraint<FixPoint>(vec2())
        ->target = p1;
    p1->drawEnable = false;
    p2->drawEnable = false;
    p3->drawEnable = false;

    world.addConstraint<Tracer>(p3, 0.1, 0.01, 0.7, 200);
}

void SetupRotateBox() {
    auto sim = Simulation::Get();
    auto& world = sim->world;
    const real worldScale = sim->getWorldUnit();
    addBox(vec2(), 3);

    auto circles = world.findObjects<Circle>();
    i32 len = circles.size();
    auto boxCenter = circles[len - 5];
    auto c1 = circles[len - 2];
    auto c2 = circles[len - 4];
    world.addConstraint<FixPoint>(vec2())
        ->target = boxCenter;

    auto h1 = world.addConstraint<Roller>(vec2(6, 0));
    auto h2 = world.addConstraint<Roller>(vec2(-6, 0));
    auto p1 = world.addRigidBody<Circle>(h1->m_position);
    auto p2 = world.addRigidBody<Circle>(h2->m_position);
    real l = glm::length(p1->m_position - c1->m_position) / worldScale;
    world.addConstraint<DistanceConstraint>(c1, p1, l);
    world.addConstraint<DistanceConstraint>(c2, p2, l);
    world.addConstraint<Spring>(p1, p2, glm::length(p1->m_position - p2->m_position) / worldScale, 1, 0.1);
    h1->target = p1;
    h2->target = p2;
}

void Simulation::update(const real& dt) {
    world.update(dt);
}

void Simulation::render(mfw::Renderer& renderer) {
    world.render(camera.getProjection(), renderer);
}

real operator""_mu(const long double value) {
    ASSERT(Simulation::Get());
    return static_cast<real>(
            Simulation::Get()->getWorldUnit() * value
            );
}

real operator""_du(const long double value) {
    ASSERT(Simulation::Get());
    return static_cast<real>(
            Simulation::Get()->getWorldUnit() * value * 0.1
            );
}

real operator""_cu(const long double value) {
    ASSERT(Simulation::Get());
    return static_cast<real>(
            Simulation::Get()->getWorldUnit() * value * 0.01
            );
}

real operator""_mmu(const long double value) {
    ASSERT(Simulation::Get());
    return static_cast<real>(
            Simulation::Get()->getWorldUnit() * value * 0.001
            );
}

void SetDefaultConfig() {
    Circle::default_color = {COLOR(0x858AA6)};
    Circle::default_radius = 0.2_mu;

    Cylinder::default_color = vec3(1.0);
    Cylinder::default_width = 0.04_mu;

    DistanceConstraint::default_color = {COLOR(0xefefef)};
    DistanceConstraint::default_w = 0.14_mu;

    Tracer::default_color = {COLOR(0xc73e3e)};
    Tracer::default_maxScale = 0.03_mu;
    Tracer::default_minScale = 0.01_mu;
    Tracer::default_dr = 0.75;
    Tracer::default_maxSamples = 450;

    FixPoint::default_color = {COLOR(0x486577)};
    FixPoint::default_d = Circle::default_radius * 1.6f;

    Roller::default_color = {COLOR(0x3c4467)};
    Roller::default_d = FixPoint::default_d;

    Spring::default_color = {COLOR(0xefefef)};
    Spring::default_w = 0.06_mu;
    Spring::default_stiffness = 14;
    Spring::default_damping = 0.3;
}

