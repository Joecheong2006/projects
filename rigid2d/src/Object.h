#pragma once

#include "glm/glm.hpp"
#include "util.h"

class ObjectTypeIdGenerator {
private:
    static i32 current;

public:
    static inline i32 GenerateId() {
        return current++;
    }

    static inline i32 GetCurrent() {
        return current;
    }

};

#define GENERATE_OBJECT_IDENTIFIER()\
    static inline i32 GetTypeId() {\
        static i32 id = ObjectTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }\

namespace mfw {
    class Renderer;
};

class Object {
public:
    glm::dvec2 m_pos, m_opos, m_velocity, m_ovelocity, m_acceleration;
    glm::dvec3 m_color;
    f64 m_mass;
    bool display = true;

    Object() {}
    Object(const glm::dvec3& color): m_color(color) {}
    Object(const glm::dvec2& pos, const f64& mass, const glm::dvec3& color);
    virtual ~Object() {}
    void addForce(const glm::dvec2& force);

    virtual inline i32 getTypeId() const { return -1; };
    virtual void update(const f64& dt);
    virtual void render(const glm::mat4& proj, mfw::Renderer& renderer) { (void)proj; (void)renderer; }

};

