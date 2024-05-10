#pragma once
#include "PointConstraint.h"
#include "RigidBody2D.h"

class Rotator : public Constraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();
    Rotator(RigidBody2D* center, RigidBody2D* target, real r = default_r, real w = default_w);

    real r, w;
    vec2 m_pos;
    RigidBody2D* center;
    RigidBody2D* target;

    static real default_r, default_w;

};

