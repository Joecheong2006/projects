#ifndef _OBJECT_H
#define _OBJECT_H

#include "glm/glm.hpp"
#include "util.h"

class Object2D {
public:
    glm::dvec2 m_pos, m_opos, m_acceleration;
    glm::dvec4 m_color;
    f64 m_mass;

    Object2D(const glm::dvec2& pos, const f64& mass, const glm::dvec4& color);
    virtual ~Object2D() {};
    virtual void update(const f64& dt);

    inline void addForce(const glm::dvec2& force) {
        m_acceleration += force / m_mass;
    }

};

#endif
