#include "RigidBody2D.h"
#include <mfw/mfwlog.h>

RigidBody2D::RigidBody2D(const vec2& pos, const real& mass, const color& color, Collider2D* collider)
    : m_position(pos)
    , m_oposition(pos)
    , m_color(color)
    , m_collider(collider)
{
    setMass(mass);
}

void RigidBody2D::addForce(const vec2& force) {
    if (isStatic)
        return;
    m_acceleration += force * m_inverse_mass;
}

void RigidBody2D::setMass(real newMass) {
    if (newMass < 0) {
        m_inverse_mass = 0;
        return;
    }
    m_mass = newMass;
    m_inverse_mass = 1.0 / newMass;
}

void RigidBody2D::setStatic() {
    isStatic = true;
    m_inverse_mass = 0;
}

void RigidBody2D::setDynamic() {
    isStatic = false;
    ASSERT(m_mass > 0);
    m_inverse_mass = 1.0 / m_mass;
}

