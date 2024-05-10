#pragma once

#include "RigidBody2D.h"
#include "Constraint.h"

namespace mfw {
    class Renderer;
};

class DistanceConstraint : public Constraint {
private:
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    DistanceConstraint(RigidBody2D* target1, RigidBody2D* target2, real d, f32 w = default_w, color color = default_color);
    virtual void update(const real& dt) override;

    real d, w;
    color color;
    RigidBody2D* target[2];
    
    static ::color default_color;
    static real default_w;

};

