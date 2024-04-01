#pragma once

#include "util.h"
#include "Drawable.h"

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

#define GENERATE_OBJECT_IDENTIFIER(identifier)\
    static inline i32 GetTypeId() {\
        static const i32 id = ObjectTypeIdGenerator::GenerateId();\
        return id;\
    }\
    virtual inline i32 getTypeId() const override {\
        return GetTypeId();\
    }\
    static inline const char* GetTypeName() {\
        static const char* name = #identifier;\
        return name;\
    }\
    virtual inline const char* getTypeName() const override {\
        return GetTypeName();\
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

    virtual inline i32 getTypeId() const { return -1; };
    virtual inline const char* getTypeName() const { return "None"; };
    virtual void update(const f64& dt);
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override { (void)proj; (void)renderer; }

};

