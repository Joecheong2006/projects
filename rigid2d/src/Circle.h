#pragma once
#include "RigidBody2D.h"

class Circle : public RigidBody {
public:
    GENERATE_OBJECT_IDENTIFIER();

    f32 r;
    Circle();
    Circle(const glm::vec2& pos, const glm::vec3& color, const f32& r);
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Circle> {
    const glm::vec3 default_color;
    const f32 default_d;
    Circle* operator()(const glm::vec2& pos, const glm::vec3& color, const f32& d);
    Circle* operator()(const glm::vec2& pos, const f32& d);
    Circle* operator()(const glm::vec2& pos);
    Circle* operator()();
};


