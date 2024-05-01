#include "Line.h"
#include "Renderer.h"
#include "Simulation.h"

Line::Line(vec2 p1, vec2 p2, real width)
    : RigidBody({}, -1, vec3(1)), width(width)
{
    RigidBody::collider = static_cast<Collider*>(&this->collider);
    p[0] = p1;
    p[1] = p2;
    setStatic();
}

void Line::draw(const mat4& proj, mfw::Renderer& renderer) {
    renderer.renderLine(proj, p[0], p[1], m_color, width);
    renderer.renderCircle(proj, p[0], width, glm::vec4(m_color, 1));
    renderer.renderCircle(proj, p[1], width, glm::vec4(m_color, 1));
}

color BuildObject<Line>::default_color;
real BuildObject<Line>::default_width;

BuildObject<Line>::BuildObject(vec2 p1, vec2 p2, real width, color color)
{
    const real worldScale = Simulation::Get()->getWorldScale();
    object = Simulation::Get()->world.addRigidBody<Line>(p1 * worldScale, p2 * worldScale, width * worldScale);
    object->m_restitution = 1;
    object->m_color = color;
}

