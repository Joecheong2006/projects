#pragma once

#include "ObjectTypeIdGenerator.h"
#include "Drawable.h"

#define GENERATE_OBJECT_IDENTIFIER()\
    static inline i32 GetTypeId() {\
        static const i32 id = ObjectTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }

class Object : public Drawable {
public:
    glm::dvec2 m_pos, m_opos, m_velocity, m_ovelocity, m_acceleration;
    glm::dvec3 m_color;
    f64 m_mass;

    Object() = default;
    Object(const glm::dvec3& color): m_color(color) {}
    Object(const glm::dvec2& pos, const f64& mass, const glm::dvec3& color);
    virtual ~Object() = default;
    void addForce(const glm::dvec2& force);

    virtual void update(const f64& dt);
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override { (void)proj; (void)renderer; }
    virtual inline i32 getTypeId() const { return -1; };
};

