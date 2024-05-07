#pragma once
#include "PointConstraint.h"

class FixPoint: public PointConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    FixPoint() = default;
    FixPoint(vec2 position, real d = default_d, color color = default_color);
    
    static color default_color;
    static real default_d;

};

