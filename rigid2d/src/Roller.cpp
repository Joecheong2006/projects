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

void Roller::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    Circle(m_position, m_color, d).draw(proj, renderer);
}

color ObjectBuilder<Roller>::default_color;
real ObjectBuilder<Roller>::default_d;

Roller* ObjectBuilder<Roller>::operator()(vec2 pos, real d, color color) {
    const f32 worldScale = Simulation::Get()->getWorldScale();
    auto roller = Simulation::Get()->world.addConstraint<Roller>();
    roller->m_position = pos;
    roller->m_color = color;
    roller->d = d * worldScale;
    return roller;
}

