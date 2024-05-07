#pragma once
#include "RigidBody2D.h"
#include "CircleCollider.h"

class Circle : public RigidBody {
public:
    GENERATE_OBJECT_IDENTIFIER();

    Circle() = default;
    Circle(vec2 position,
           real radius = default_radius,
           color color = default_color);

    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;
    CircleCollider collider;
    real radius;

    static color default_color;
    static real default_radius;

};

