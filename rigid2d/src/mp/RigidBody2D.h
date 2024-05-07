#pragma once
#include "Object.h"
#include "Collider.h"

class RigidBody : public Object {
public:
    RigidBody() = default;
    RigidBody(const vec2& pos, const real& mass, const color& color);
    ~RigidBody();
    void addForce(const vec2& force);
    void setMass(real newMass);
    void setStatic();
    void setDynamic();
    vec2 getForce();

    vec2 m_position,
         m_oposition,
         m_velocity,
         m_ovelocity,
         m_acceleration,
         m_oacceleration;

    real m_angle,
         m_angular_velocity,
         m_angular_acceleration,
         m_mass,
         m_inverse_mass,
         m_restitution,
         m_damping;

    color m_color;
    bool isStatic;

    Collider* collider;

};

