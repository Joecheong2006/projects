#pragma once

#include "RigidBody2D.h"
#include "Constraint.h"

namespace mfw {
    class Renderer;
};

class DistanceConstraint : public Constraint {
private:
    virtual void update(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();

    DistanceConstraint(RigidBody* t1, RigidBody* t2, f32 d, f32 w);

    f32 d, w;
    RigidBody* target[2];
    glm::dvec3 m_color;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<DistanceConstraint> {
    const glm::vec3 default_color;
    const f32 default_w;
    DistanceConstraint* operator()(RigidBody* target1, RigidBody* target2, f32 d, f32 w, glm::vec3 color);
    DistanceConstraint* operator()(RigidBody* target1, RigidBody* target2, f32 d);
};

