#pragma once
#include "PointConstraint.h"

class Roller: public PointConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    Roller() = default;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Roller> {
    const color default_color;
    const real default_d;
    Roller* operator()(vec2 pos, real d, color color);
    Roller* operator()(vec2 pos);
};

