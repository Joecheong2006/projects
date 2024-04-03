#pragma once
#include "PointConstraint.h"
#include "Object.h"
#include <mfwlog.h>

class Rotator : public PointConstraint {
public:
    GENERATE_CONSTRAINT_IDENTIFIER();
    Rotator() = default;
    virtual void solve(const f64& dt) override;
    inline virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override
    { TOVOID(proj, renderer); };

    f32 r, w;
    Object* center;

};
