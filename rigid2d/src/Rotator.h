#pragma once
#include "PointConstraint.h"
#include "RigidBody2D.h"

class Rotator : public Constraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();
    Rotator(RigidBody* center, RigidBody* target);

    real  r, w;
    vec2 m_pos;
    RigidBody* center;
    RigidBody* target;

};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Rotator> {
    static real default_r, default_w;
    Rotator* operator()(RigidBody* center, RigidBody* target, real r = default_r, real w = default_w);
};

