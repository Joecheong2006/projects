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
    static color default_color;
    static real default_maxScale, default_minScale, default_dr;
    static i32 default_maxSamples;
    Tracer* operator()(RigidBody* target,
            real maxScale = default_maxScale,
            real minScale = default_minScale,
            real dr = default_dr,
            i32 maxSamples = default_maxSamples,
            color color = default_color);
};

