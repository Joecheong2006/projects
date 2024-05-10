#pragma once
#include "DistanceConstraint.h"

class Spring : public DistanceConstraint {
private:
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Spring(RigidBody2D* target1, RigidBody2D* target2,
           real d,
           real stiffness = default_stiffness,
           real damping = default_damping,
           i32 count = default_count,
           real w = default_w,
           ::color color = default_color);
    virtual void update(const real& dt) override;
    real stiffness, damping;
    i32 count = 4;

    static ::color default_color;
    static real default_w, default_width;
    static real default_stiffness, default_damping;
    static i32 default_count;

};

