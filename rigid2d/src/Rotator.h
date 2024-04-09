#pragma once
#include "PointConstraint.h"
#include "RigidBody2D.h"

class Rotator : public Constraint {
private:
    virtual void update(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    GENERATE_OBJECT_IDENTIFIER();
    Rotator(RigidBody* center, RigidBody* target);

    f32 r, w;
    RigidBody* center;
    RigidBody* target;
    glm::dvec2 m_pos;

};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Rotator> {
    const f32 default_r, default_w;
    Rotator* operator()(RigidBody* center, RigidBody* target, f32 r, f32 w);
    Rotator* operator()(RigidBody* center, RigidBody* target);
};

