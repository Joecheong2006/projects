#pragma once

#include "RigidBody2D.h"
#include "Constraint.h"

namespace mfw {
    class Renderer;
};

class DistanceConstraint : public Constraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    DistanceConstraint(RigidBody* t1, RigidBody* t2, real d, real w);

    real d, w;
    color color;
    RigidBody* target[2];
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<DistanceConstraint> {
    const color default_color;
    const real default_w;
    DistanceConstraint* operator()(RigidBody* target1, RigidBody* target2, real d, f32 w, color color);
    DistanceConstraint* operator()(RigidBody* target1, RigidBody* target2, real d);
};

