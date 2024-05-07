#include "Cylinder.h"
#include "Renderer.h"

color Cylinder::default_color;
real Cylinder::default_width;

Cylinder::Cylinder(vec2 p1, vec2 p2, real width, color color)
    : RigidBody({}, 0, color), width(width)
{
    RigidBody::collider = static_cast<Collider*>(&this->collider);
    p[0] = p1;
    p[1] = p2;
    setStatic();
}

void Cylinder::draw(const mat4& proj, mfw::Renderer& renderer) {
    renderer.renderLine(proj, p[0], p[1], m_color, width);
    renderer.renderCircle(proj, p[0], width, glm::vec4(m_color, 1));
    renderer.renderCircle(proj, p[1], width, glm::vec4(m_color, 1));
}

