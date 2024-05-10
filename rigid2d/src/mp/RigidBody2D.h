#pragma once
#include "Object.h"
#include "Collider2D.h"

class RigidBody2D : public Object {
public:
    RigidBody2D() = default;
    RigidBody2D(const vec2& pos, const real& mass, const color& color, Collider2D* collider);
    void addForce(const vec2& force);
    void setMass(real newMass);
    void setStatic();
    void setDynamic();

    inline vec2 getForce() const { return m_acceleration * m_mass; }
    inline real getMass() const { return m_mass; }
    inline real getInverseMass() const { return m_inverse_mass; }
    inline Collider2D& getCollider() const { return *m_collider; }

    vec2 m_position{},
         m_oposition{},
         m_velocity{},
         m_ovelocity{},
         m_acceleration{},
         m_oacceleration{};

    real m_angle{},
         m_angular_velocity{},
         m_angular_acceleration{},
         m_restitution{1},
         m_damping{};

    color m_color;
    bool isStatic = false;

private:
    Collider2D* m_collider;
    real m_mass, m_inverse_mass;

};

