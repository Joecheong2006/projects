#include "Object.h"
#include "logger.h"

Object2D::Object2D(const glm::dvec2& pos, const f64& mass, const glm::dvec4& color) 
    : m_pos(pos), m_opos(pos), m_acceleration(0), m_color(color), m_mass(mass)
{}

void Object2D::update(const f64& dt) {
    auto s = (m_pos - m_opos);
    m_opos = m_pos;
    m_pos += s + m_acceleration * dt * dt * 0.5;
    //m_acceleration = ((m_pos - m_opos) - s) / dt;
    m_acceleration = glm::dvec2(0);
}
