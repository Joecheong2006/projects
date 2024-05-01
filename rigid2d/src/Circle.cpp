#include "Circle.h"
#include "Renderer.h"
#include "Simulation.h"

Circle::Circle(const vec2& position, const color& color, const real& r)
    : RigidBody(position, r, color), radius(r)
{
    RigidBody::collider = static_cast<Collider*>(&this->collider);
}

void Circle::draw(const mat4& proj, mfw::Renderer& renderer) {
    const real worldScale = Simulation::Get()->getWorldScale();
    renderer.renderCircleI(proj, m_position, radius, glm::vec4(0, 0, 0, 1));
    renderer.renderCircleI(proj, m_position, radius - worldScale * 0.03, glm::vec4(m_color, 1));

    const vec2 direction = vec2(cos(m_angle), sin(m_angle));
    renderer.renderCircleI(proj, m_position + direction * radius * 0.6, radius * 0.1, glm::vec4(0, 0, 0, 1));
}

color BuildObject<Circle>::default_color;
real BuildObject<Circle>::default_d;
real BuildObject<Circle>::default_restitution;

BuildObject<Circle>::BuildObject(vec2 position, real d, real restitution, color color)
{
    const real worldScale = Simulation::Get()->getWorldScale();
    object = Simulation::Get()->world.addRigidBody<Circle>(position * worldScale, color, d * worldScale);
    object->m_restitution = restitution;
}

