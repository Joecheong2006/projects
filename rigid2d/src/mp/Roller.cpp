#include "Roller.h"
#include "Circle.h"

color Roller::default_color;
real Roller::default_d;

Roller::Roller(vec2 pos, real d, color color)
{
    m_position = pos;
    m_color = color;
    this->d = d;
}

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

