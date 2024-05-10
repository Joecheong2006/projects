#include "Cylinder.h"
#include "Renderer.h"

color Cylinder::default_color;
real Cylinder::default_width;

Cylinder::Cylinder(vec2 p1, vec2 p2, real width, color color)
    : RigidBody2D({}, 0, color, &collider), width(width)
{
    p[0] = p1;
    p[1] = p2;
    m_position = (p1 + p2) * 0.5;
    setStatic();
    collider.offset = {};
    collider.size = {glm::length(p1 - p2), width};
    collider.owner = this;
}

void Cylinder::draw(const mat4& proj, mfw::Renderer& renderer) {
    renderer.renderLine(proj, p[0], p[1], m_color, width);
    renderer.renderCircle(proj, p[0], width, glm::vec4(m_color, 1));
    renderer.renderCircle(proj, p[1], width, glm::vec4(m_color, 1));
}

