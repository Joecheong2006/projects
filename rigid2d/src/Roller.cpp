#include "Roller.h"
#include "Circle.h"
#include "Simulation.h"

void Roller::update([[maybe_unused]] const real& dt) {
    if (!target)
        return;
    m_position.x = target->m_position.x;
    target->m_position.y = m_position.y;
    target->m_acceleration.y = 0;
    target->m_velocity.y = 0;
}

void Roller::draw(const mat4& proj, mfw::Renderer& renderer) {
    Circle(m_position, d, m_color).draw(proj, renderer);
}

color BuildObject<Roller>::default_color;
real BuildObject<Roller>::default_d;

BuildObject<Roller>::BuildObject(vec2 pos, real d, color color) {
    const f32 worldScale = Simulation::Get()->getWorldUnit();
    object = Simulation::Get()->world.addConstraint<Roller>();
    object->m_position = pos;
    object->m_color = color;
    object->d = d * worldScale;
}

