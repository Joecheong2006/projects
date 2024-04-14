#include "Simulation.h"

#include "ObjectBuilder.h"

#include "Renderer.h"
#include <mfw/mfwlog.h>
#include <mfw/Application.h>
#include <mfw/Input.h>

Simulation::Simulation(const std::string& name, real worldScale)
    : name(name), unitScale(worldScale)
{
    initialize = [this]() {
        world.initialize();
        auto buildFixPoint = ObjectBuilder<FixPoint>();
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
    return camera.view * (uv * ((glm::dvec4(1.0) / camera.ortho) / camera.scale));
}

void addString(const vec2& pos, u32 node, real length) {
    ASSERT(node > 1);

    auto buildCircle = ObjectBuilder<Circle>();
    auto buildLink = ObjectBuilder<DistanceConstraint>();
    auto p1 = buildCircle(pos);
    for (real i = 1; i < node; i++) {
        auto p2 = buildCircle(vec2(0, -i * length) + pos);
        buildLink(p1, p2, length);
        p1 = p2;
    }
}

void addCircle(const vec2& pos, i32 n, real r, i32 nstep) {
    ASSERT(r > 0 && n > 2 && nstep > 0);
    static const real pi = 3.14159265359f;

    real ri = 2 * pi / n;
    real nlen = glm::length(r * vec2(sin(0) - sin(ri), cos(0) - cos(ri)));

    auto buildCircle = ObjectBuilder<Circle>();
    auto buildLink = ObjectBuilder<DistanceConstraint>();
    auto center = buildCircle(pos);

    RigidBody* p1 = buildCircle(r * vec2(sin(0), cos(0)) + pos);
    RigidBody* first = p1;
    RigidBody* p2;

    for (i32 i = 1; i < n; i++) {
        real a = ri * i;
        p2 = buildCircle(r * vec2(sin(a), cos(a)) + pos);
        buildLink(p1, center, r);
        buildLink(p1, p2, nlen);
        p1 = p2;
    }

    buildLink(p1, center, r);
    buildLink(p1, first, nlen);
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
    real r = angle * 3.14 / 180;

    auto buildCircle = ObjectBuilder<Circle>();
    auto buildLink = ObjectBuilder<DistanceConstraint>();
    auto buildFixPoint = ObjectBuilder<FixPoint>();
    vec2 direction = glm::normalize(vec2(cos(r), sin(r))) * d;

    auto p1 = buildCircle({});
    auto p2 = buildCircle(direction);
    auto p3 = buildCircle(direction * 2.0);
    buildLink(p1, p2, d);
    buildLink(p2, p3, d);
    buildFixPoint({})
        ->target = p1;
    p1->drawEnable = false;
    p2->drawEnable = false;
    p3->drawEnable = false;

    auto buildTracer = ObjectBuilder<Tracer>();
    buildTracer(p3, 0.1, 0.01, 0.7, 200);
}

void SetupRotateBox() {
    auto sim = Simulation::Get();
    auto& world = sim->world;
    const real worldScale = sim->getWorldScale();
    addBox(vec2(), 3);

    auto buildCircle = ObjectBuilder<Circle>();
    auto buildLink = ObjectBuilder<DistanceConstraint>();
    auto buildFixPoint = ObjectBuilder<FixPoint>();
    auto buildRoller = ObjectBuilder<Roller>();
    auto buildSpring = ObjectBuilder<Spring>();

    auto& circles = world.getObjects<Circle>();
    i32 len = circles.size();
    auto boxCenter = circles[len - 5];
    auto c1 = circles[len - 2];
    auto c2 = circles[len - 4];
    buildFixPoint({})
        ->target = boxCenter;

    auto h1 = buildRoller(vec2(6, 0));
    auto h2 = buildRoller(vec2(-6, 0));
    auto p1 = buildCircle(h1->m_position);
    auto p2 = buildCircle(h2->m_position);
    real l = glm::length(p1->m_position - c1->m_position) / worldScale;
    buildLink(c1, p1, l);
    buildLink(c2, p2, l);
    buildSpring(p1, p2, glm::length(p1->m_position - p2->m_position) / worldScale, 1, 0.1);
    h1->target = p1;
    h2->target = p2;
}

