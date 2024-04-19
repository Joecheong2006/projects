#pragma once

#include "RigidBody2D.h"
#include "Constraint.h"
#include "ObjectBuilderSet.h"

namespace mfw {
    class Renderer;
};

class DistanceConstraint : public Constraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    DistanceConstraint(RigidBody* t1, RigidBody* t2, real d, real w);

    real d, w;
    color color;
    RigidBody* target[2];
    
};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<DistanceConstraint> : public ObjectBuilderSet<DistanceConstraint, Constraint> {
    static color default_color;
    static real default_w;
    BuildObject(RigidBody* target1, RigidBody* target2, real d, f32 w = default_w, color color = default_color);
};

