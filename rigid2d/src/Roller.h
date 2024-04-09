#pragma once
#include "PointConstraint.h"

class Roller: public PointConstraint {
private:
    virtual void update(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

public:
    Roller() = default;
    
};

template <typename T>
struct ObjectBuilder;

template <>
struct ObjectBuilder<Roller> {
    const glm::vec3 default_color;
    const f32 default_d;
    Roller* operator()(glm::vec2 pos, f32 d, glm::vec3 color);
    Roller* operator()(glm::vec2 pos);
};

