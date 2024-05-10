#pragma once
#include "RigidBody2D.h"
#include "CylinderCollider.h"

class Cylinder : public RigidBody2D {
public:
    GENERATE_OBJECT_IDENTIFIER();

    Cylinder(vec2 p1, vec2 p2, real width = default_width, color color = default_color);
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

    vec2 p[2];
    real width;
    CylinderCollider2D collider;

    static color default_color;
    static real default_width;

};

