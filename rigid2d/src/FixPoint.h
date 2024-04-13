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
    const glm::vec3 default_color;
    const real default_d;
    FixPoint* operator()(vec2 position, real d, color color);
    FixPoint* operator()(vec2 position);
};

