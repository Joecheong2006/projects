#pragma once

#include "Object.h"
#include "Constraint.h"

namespace mfw {
    class Renderer;
};

class DistanceConstraint : public Constraint {
public:
    GENERATE_CONSTRAINT_IDENTIFIER();

    DistanceConstraint(Object* t1, Object* t2, f32 d, f32 w);
    virtual void solve(const f64& dt) override;
    virtual void draw(const glm::mat4& proj, mfw::Renderer& renderer) override;

    Object* target[2];
    f32 d, w;
    f64 hardness = 1;
    glm::vec3 color;
    
};
