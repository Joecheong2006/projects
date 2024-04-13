#pragma once
#include "RigidBody2D.h"
#include "Constraint.h"
#include <list>

class Tracer: public Constraint {
private:
    std::list<vec2> positions_trace;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Tracer(RigidBody* target);

    i32 maxSamples = 100;
    real maxScale, minScale;
    real dr = 0.6f;
    color m_color;

    RigidBody* target;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Tracer> {
    const color default_color;
    const real default_maxScale, default_minScale, default_dr;
    const i32 default_maxSamples;
    Tracer* operator()(RigidBody* target, real maxScale, real minScale, real dr, i32 maxSamples, glm::vec3 color);
    Tracer* operator()(RigidBody* target);
};

