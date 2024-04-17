#include "Circle.h"
#include "Renderer.h"
#include "Simulation.h"

Circle::Circle(const vec2& position, const color& color, const real& r)
    : RigidBody(position, r, color), radius(r)
{}

Circle::Circle()
    : RigidBody(glm::vec2(0), 1, glm::vec4(1)), radius(1)
{}

void Circle::draw(const mat4& proj, mfw::Renderer& renderer) {
    const real worldScale = Simulation::Get()->getWorldScale();
    renderer.renderCircle(proj, m_position, radius, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, m_position, radius - worldScale * 0.03, glm::vec4(m_color, 1));

    const vec2 direction = vec2(cos(m_angle), sin(m_angle));
    renderer.renderCircle(proj, m_position + direction * radius * 0.6, radius * 0.1, glm::vec4(0, 0, 0, 1));
}

color ObjectBuilder<Circle>::default_color;
real ObjectBuilder<Circle>::default_d;

Circle* ObjectBuilder<Circle>::operator()(const vec2& position, const real& d, const color& color) {
    const real worldScale = Simulation::Get()->getWorldScale();
    auto circle = Simulation::Get()->world.addRigidBody<Circle>(position * worldScale, color, d * worldScale);
    return circle;
}

