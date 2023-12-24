#include "Object.h"
#include <iostream>

Object2D::Object2D(const glm::vec2& pos, const f32& mass, const glm::vec3& color) 
    : m_pos(pos), m_opos(pos), m_acceleration(0), m_color(color), m_mass(mass)
{}

void Object2D::update(const f32& frame) {
    auto m_velocity = (m_pos - m_opos);
    m_opos = m_pos;
    m_pos += m_velocity + m_acceleration * frame * frame * 0.5f;
    m_acceleration = glm::vec2(0);
}
