#pragma once
#include "RigidBody2D.h"

class Circle : public RigidBody {
public:
    GENERATE_OBJECT_IDENTIFIER();

    real radius;
    Circle();
    Circle(const vec2& position, const color& color, const real& r);
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Circle> {
    static color default_color;
    static real default_d;
    Circle* operator()(const vec2& position, const real& d = default_d, const color& color = default_color);
};

