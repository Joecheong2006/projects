#include "Circle.h"
#include "Renderer.h"

color Circle::default_color;
real Circle::default_radius;

Circle::Circle(vec2 position, real radius, color color)
    : RigidBody(position, radius, color), radius(radius)
{
    RigidBody::collider = static_cast<Collider*>(&this->collider);
}

void Circle::draw(const mat4& proj, mfw::Renderer& renderer) {
    renderer.renderCircleI(proj, m_position, radius, glm::vec4(0, 0, 0, 1));
    renderer.renderCircleI(proj, m_position, radius * 0.9, glm::vec4(m_color, 1));

    const vec2 direction = vec2(cos(m_angle), sin(m_angle));
    renderer.renderCircleI(proj, m_position + direction * radius * 0.6, radius * 0.1, glm::vec4(0, 0, 0, 1));
}

