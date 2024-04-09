#pragma once
#include "PointConstraint.h"

class FixPoint: public PointConstraint {
private:
    virtual void update(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    FixPoint() = default;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<FixPoint> {
    const glm::vec3 default_color;
    const f32 default_d;
    FixPoint* operator()(glm::vec2 position, f32 d, glm::vec3 color);
    FixPoint* operator()(glm::vec2 position);
};

