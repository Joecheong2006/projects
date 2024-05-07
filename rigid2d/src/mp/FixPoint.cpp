#include "FixPoint.h"
#include "Circle.h"

color FixPoint::default_color;
real FixPoint::default_d;

FixPoint::FixPoint(vec2 pos, real d, color color)
{
    m_position = pos;
    m_color = color;
    this->d = d;
}

void FixPoint::update([[maybe_unused]] const real& dt) {
    if (!target)
        return;
    target->m_position = m_position;
}

void FixPoint::draw(const mat4& proj, mfw::Renderer& renderer) {
    Circle(m_position, d, m_color).draw(proj, renderer);
}

