#pragma once
#include "PointConstraint.h"

class FixPoint: public PointConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    FixPoint() = default;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<FixPoint> {
    static color default_color;
    static real default_d;
    FixPoint* operator()(vec2 position, real d = default_d, color color = default_color);
};

