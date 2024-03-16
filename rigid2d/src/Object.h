#pragma once

#include "glm/glm.hpp"
#include "util.h"

#define SET_OBJECT_NORMAL_BEHAVIOUR(type) \
    static ObjectType GetType() {\
        return type;\
    }\
    virtual inline ObjectType getType() override {\
        return type;\
    }

enum class ObjectType {
    Circle,
    Ractangle,
    None
};

class Object {
public:
    glm::dvec2 m_pos, m_opos, m_velocity, m_acceleration;
    glm::dvec3 m_color;
    f64 m_mass;

    Object() {}
    Object(const glm::dvec3& color): m_color(color) {}
    Object(const glm::dvec2& pos, const f64& mass, const glm::dvec3& color);
    virtual ~Object() {}
    void addForce(const glm::dvec2& force);

    virtual inline ObjectType getType() { return ObjectType::None; };
    virtual void update(const f64& dt);

};

