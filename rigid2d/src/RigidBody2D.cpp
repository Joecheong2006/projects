#include "RigidBody2D.h"
#include <mfw/mfwlog.h>

RigidBody::RigidBody(const vec2& pos, const real& mass, const color& color) 
    : m_position(pos)
    , m_oposition(pos)
    , m_velocity(0)
    , m_ovelocity(0)
    , m_acceleration(0)
    , m_angle(0)
    , m_angular_velocity(0)
    , m_angular_acceleration(0)
    , m_restitution(1)
    , m_damping(0)
    , m_color(color)
    , isStatic(false)
{
    setMass(mass);
}

RigidBody::~RigidBody() {
}

void RigidBody::addForce(const vec2& force) {
    if (isStatic)
        return;
    m_acceleration += force * m_inverse_mass;
}

void RigidBody::setMass(real newMass) {
    if (newMass < 0) {
        m_inverse_mass = 0;
        return;
    }
    m_mass = newMass;
    m_inverse_mass = 1.0 / newMass;
}

void RigidBody::setStatic() {
    isStatic = true;
    m_inverse_mass = 0;
}

void RigidBody::setDynamic() {
    isStatic = false;
    ASSERT(m_mass > 0);
    m_inverse_mass = 1.0 / m_mass;
}

vec2 RigidBody::getForce() {
    if (isStatic)
        return {};
    return m_acceleration * m_mass;
}

