#pragma once
#include "DistanceConstraint.h"
#include "ObjectBuilderSet.h"

class Spring : public DistanceConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Spring(RigidBody* t1, RigidBody* t2, real d, real w);
    real stiffness, damping;
    i32 count = 4;

};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<Spring> : public ObjectBuilderSet<Spring, Constraint> {
    static color default_color;
    static real default_w;
    static real default_stiffness, default_damping;
    static i32 default_count;
    BuildObject(RigidBody* target1, RigidBody* target2,
                  real d,
                  real stiffness = default_stiffness,
                  real damping = default_damping,
                  i32 count = default_count,
                  real w = default_w,
                  color color = default_color);
};

