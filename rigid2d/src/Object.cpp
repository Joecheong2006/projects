#include "Object.h"
#include "logger.h"

Object2D::Object2D(const glm::dvec2& pos, const f64& mass, const glm::dvec4& color) 
    : m_pos(pos), m_opos(pos), m_velocity(0), m_acceleration(0), m_color(color), m_mass(mass)
{}

void Object2D::addForce(const glm::dvec2& force) {
    m_acceleration += force / m_mass;
}

void Object2D::update(const f64& dt) {
    m_velocity = (m_pos - m_opos) / dt;
    m_opos = m_pos;
    m_pos += m_velocity * dt + m_acceleration * dt * dt * 0.5;
    m_acceleration = ((m_pos - m_opos) / dt - m_velocity) / dt;
}
