#pragma once
#include "PointConstraint.h"

class Roller: public PointConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    Roller() = default;
    Roller(vec2 pos, real d = default_d, color color = default_color);
    
    static color default_color;
    static real default_d;

};

