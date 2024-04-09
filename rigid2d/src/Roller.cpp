#include "Roller.h"
#include "Circle.h"
#include "Simulation.h"

void Roller::update([[maybe_unused]] const f64& dt) {
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

Roller* ObjectBuilder<Roller>::operator()(glm::vec2 pos, f32 d, glm::vec3 color) {
    static const f32 worldScale = Simulation::Get()->getWorldScale();
    auto roller = Simulation::Get()->world.addConstraint<Roller>();
    roller->m_position = pos;
    roller->m_color = color;
    roller->d = d * worldScale;
    return roller;
}
Roller* ObjectBuilder<Roller>::operator()(glm::vec2 pos) {
    return (*this)(pos, default_d, default_color);
}

