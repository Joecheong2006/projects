#include "RigidBody2D.h"

RigidBody::RigidBody(const color& color)
    : m_color(color)
{}

RigidBody::RigidBody(const vec2& pos, const real& mass, const color& color) 
    : m_opos(pos)
    , m_position(pos)
    , m_velocity(0)
    , m_ovelocity(0)
    , m_acceleration(0)
    , m_mass(mass)
    , m_color(color)
{}

void RigidBody::addForce(const vec2& force) {
    m_acceleration += force / m_mass;
}

