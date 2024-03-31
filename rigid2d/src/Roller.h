#pragma once

#include "PointConstraint.h"
#include "Object.h"

class Roller: public PointConstraint {
public:
    Roller() = default;
    virtual void solve(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;
    
};
