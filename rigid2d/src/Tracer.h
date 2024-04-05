#pragma once
#include "RigidBody2D.h"
#include "Constraint.h"
#include <list>

class Tracer: public Constraint {
private:
    std::list<glm::vec2> positions_trace;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Tracer(RigidBody* target);
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

    i32 maxSamples = 100;
    f32 maxScale, minScale;
    f32 dr = 0.6f;

    RigidBody* target;
    glm::vec3 m_color;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Tracer> {
    const glm::vec3 default_color;
    const f32 default_maxScale, default_minScale, default_dr;
    const i32 default_maxSamples;
    Tracer* operator()(RigidBody* target, f32 maxScale, f32 minScale, f32 dr, i32 maxSamples, glm::vec3 color);
    Tracer* operator()(RigidBody* target);
};

