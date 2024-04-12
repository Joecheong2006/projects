#pragma once

#include "DistanceConstraint.h"

class Spring : public DistanceConstraint {
private:
    virtual void update(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    Spring(RigidBody* t1, RigidBody* t2, f32 d, f32 w);
    f64 stiffness, damping;

};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Spring> {
    const glm::vec3 default_color;
    const f32 default_w;
    const f64 default_stiffness, default_damping;
    Spring* operator()(RigidBody* target1, RigidBody* target2, f32 d, f32 w, glm::vec3 color, f64 stiffness, f64 damping);
    Spring* operator()(RigidBody* target1, RigidBody* target2, f32 d, f64 stiffness, f64 damping);
    Spring* operator()(RigidBody* target1, RigidBody* target2, f32 d);
};
