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
    const color default_color = color(0.52, 0.54, 0.65);
    const real default_d = 0.17;
    Circle* operator()(const vec2& position, const real& d, const color& color);
    Circle* operator()(const vec2& position, const real& d);
    Circle* operator()(const vec2& position = color());
};

