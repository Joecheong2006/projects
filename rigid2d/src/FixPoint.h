#pragma once

#include "PointConstraint.h"
#include "Object.h"

class FixPoint: public PointConstraint {
public:
    FixPoint() = default;
    virtual void solve(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;
    
};

