#pragma once
#include "RigidBody2D.h"
#include "ObjectBuilderSet.h"
#include "CircleCollider.h"

class Circle : public RigidBody {
public:
    GENERATE_OBJECT_IDENTIFIER();

    Circle() = default;
    Circle(const vec2& position, const color& color, const real& r);

    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;
    real radius;
    CircleCollider collider;

};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<Circle> : public ObjectBuilderSet<Circle, RigidBody> {
    static color default_color;
    static real default_d, default_restitution;
    BuildObject(vec2 position,
                real d = default_d,
                real restitution = default_restitution,
                color color = default_color);
};

