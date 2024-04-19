#include "FixPoint.h"
#include "Circle.h"
#include "Simulation.h"

void FixPoint::update([[maybe_unused]] const real& dt) {
    if (!target)
        return;
    target->m_position = m_position;
    target->m_acceleration = {};
    target->m_velocity = {};
}

void FixPoint::draw(const mat4& proj, mfw::Renderer& renderer) {
    Circle(m_position, m_color, d).draw(proj, renderer);
}

color BuildObject<FixPoint>::default_color;
real BuildObject<FixPoint>::default_d;

BuildObject<FixPoint>::BuildObject(vec2 pos, real d, color color) {
    const real worldScale = Simulation::Get()->getWorldScale();
    object = Simulation::Get()->world.addConstraint<FixPoint>();
    object->m_position = pos;
    object->m_color = color;
    object->d = d * worldScale;
}

