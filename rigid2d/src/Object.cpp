#include "Object.h"
#include "logger.h"
#include "PhysicsEmulator.h"
#include "mfwlog.h"

i32 ObjectTypeIdGenerator::current;

Object::Object(const glm::dvec2& pos, const f64& mass, const glm::dvec3& color) 
    : m_pos(pos), m_opos(pos), m_velocity(0), m_ovelocity(0), m_acceleration(0), m_color(color), m_mass(mass)
{}

void Object::addForce(const glm::dvec2& force) {
    m_acceleration += force / m_mass;
}

void Object::update(const f64& dt) {
#if 1
    m_opos = m_pos;
    m_ovelocity = m_velocity;

    m_velocity += m_acceleration * dt;
    m_pos += m_velocity * dt + m_acceleration * dt * dt * 0.5;

    // m_velocity += m_acceleration * dt;
    // m_pos += (m_velocity) * dt;
#else
    glm::dvec2 s = (m_pos - m_opos);
    m_opos = m_pos;
    m_pos += s + m_acceleration * dt * dt * 0.5;
    m_velocity = (m_pos - m_opos) / dt;
#endif
    m_acceleration = glm::dvec2(0);
}
