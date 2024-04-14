#pragma once
#include "util.h"
#include "Object.h"

class RigidBody : public Object {
public:
    vec2 m_opos, m_position, m_velocity, m_ovelocity, m_acceleration;
    real m_mass;
    color m_color;

    RigidBody() = default;
    RigidBody(const color& color);
    RigidBody(const vec2& pos, const real& mass, const color& color);
    void addForce(const vec2& force);

};

