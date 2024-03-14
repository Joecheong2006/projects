#pragma once

#include "glm/glm.hpp"
#include "util.h"

class Object2D {
public:
    glm::dvec2 m_pos, m_opos, m_velocity, m_acceleration, m_force;
    glm::dvec4 m_color;
    f64 m_mass;

    Object2D(const glm::dvec2& pos, const f64& mass, const glm::dvec4& color);
    virtual ~Object2D() {};
    virtual void update(const f64& dt);
    void addForce(const glm::dvec2& force);

};

