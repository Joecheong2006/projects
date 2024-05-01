#pragma once
#include "RigidBody2D.h"
#include "ObjectBuilderSet.h"
#include "LineCollider.h"

class Line : public RigidBody {
public:
    GENERATE_OBJECT_IDENTIFIER();

    Line(vec2 p1, vec2 p2, real width);
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

    vec2 p[2];
    real width;
    LineCollider collider;

};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<Line> : public ObjectBuilderSet<Line, RigidBody> {
    static color default_color;
    static real default_width;
    BuildObject(vec2 p1, vec2 p2,
                real width = default_width,
                color color = default_color);
};

