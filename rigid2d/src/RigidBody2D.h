#pragma once
#include "Object.h"
#include "Transform.h"

class RigidBody : public Object {
public:
    vec2 m_opos, m_position, m_velocity, m_ovelocity, m_acceleration;
    color m_color;
    real m_mass;

    RigidBody() = default;
    RigidBody(const color& color);
    RigidBody(const vec2& pos, const real& mass, const color& color);
    void addForce(const vec2& force);

};

