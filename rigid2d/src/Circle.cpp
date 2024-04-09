#include "Circle.h"
#include "Renderer.h"
#include "Simulation.h"

Circle::Circle(const glm::vec2& position, const glm::vec3& color, const f32& r)
    : RigidBody(position, r, color), radius(r)
{}

Circle::Circle()
    : RigidBody(glm::vec2(0), 1, glm::vec4(1)), radius(1)
{}

void Circle::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    f32 worldScale = Simulation::Get()->getWorldScale();
    renderer.renderCircle(proj, m_position, radius, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, m_position, radius - worldScale * 0.03, glm::vec4(m_color, 1));
}

Circle* ObjectBuilder<Circle>::operator()(const glm::vec2& position, const glm::vec3& color, const f32& d) {
    static const f32 worldScale = Simulation::Get()->getWorldScale();
    auto circle = Simulation::Get()->world.addRigidBody<Circle>(position * worldScale, color, d * worldScale);
    return circle;
}

Circle* ObjectBuilder<Circle>::operator()(const glm::vec2& position, const f32& d) {
    return (*this)(position, default_color, d);
}
Circle* ObjectBuilder<Circle>::operator()(const glm::vec2& position) {
    return (*this)(position, default_color, default_d);
}
Circle* ObjectBuilder<Circle>::operator()() {
    return (*this)({}, default_color, default_d);
}
