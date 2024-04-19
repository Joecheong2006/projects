#pragma once
#include "RigidBody2D.h"
#include "ObjectBuilderSet.h"

class Circle : public RigidBody {
public:
    GENERATE_OBJECT_IDENTIFIER();

    real radius;
    Circle();
    Circle(const vec2& position, const color& color, const real& r);

    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<Circle> : public ObjectBuilderSet<Circle, RigidBody> {
    static color default_color;
    static real default_d;
    BuildObject(const vec2& position,
                  const real& d = default_d,
                  const color& color = default_color);
};

