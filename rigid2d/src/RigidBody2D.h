#pragma once
#include "Object.h"

class RigidBody : public Object {
private:
    friend class World;
    virtual void update(const f64& dt) override;

public:
    glm::dvec2 m_opos, m_position, m_velocity, m_ovelocity, m_acceleration;
    glm::dvec3 m_color;
    f64 m_mass;

    RigidBody() = default;
    RigidBody(const glm::dvec3& color);
    RigidBody(const glm::dvec2& pos, const f64& mass, const glm::dvec3& color);
    void addForce(const glm::dvec2& force);

};

