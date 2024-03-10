#ifndef _OBJECT_H
#define _OBJECT_H

#include "glm/glm.hpp"
#include "util.h"

class Object2D {
public:
    glm::vec2 m_pos, m_opos, m_acceleration;
    glm::vec4 m_color;
    f32 m_mass;

    Object2D(const glm::vec2& pos, const f32& mass, const glm::vec4& color);
    virtual ~Object2D() {};
    virtual void update(const f32& frame);

    inline void move(const f32& dx, const f32& dy) {
        m_pos.x += dx;
        m_pos.y += dy;
    }

    inline void addForce(const glm::vec2& force) {
        m_acceleration += force / m_mass;
    }

    const glm::vec2& pos() const { return m_pos; }
    const glm::vec4& color() const { return m_color; }
};

#endif
