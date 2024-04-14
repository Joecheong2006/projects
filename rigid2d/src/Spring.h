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
    static color default_color;
    static real default_w;
    static real default_stiffness, default_damping;
    Spring* operator()(RigidBody* target1, RigidBody* target2,
            real d,
            real stiffness = default_stiffness,
            real damping = default_damping,
            real w = default_w,
            color color = default_color);
};
