#pragma once

#include "DistanceConstraint.h"

class Spring : public DistanceConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Spring(RigidBody* t1, RigidBody* t2, real d, real w);
    real stiffness, damping;

};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Spring> {
    const color default_color;
    const real default_w;
    const real default_stiffness, default_damping;
    Spring* operator()(RigidBody* target1, RigidBody* target2, real d, real w, color color, real stiffness, real damping);
    Spring* operator()(RigidBody* target1, RigidBody* target2, real d, real stiffness, real damping);
    Spring* operator()(RigidBody* target1, RigidBody* target2, real d);
};
