#pragma once

#include "Object.h"
#include "Constraint.h"

namespace mfw {
    class Renderer;
};

class Attribute;
class DistanceConstraint : public Constraint {
public:
    SET_CONSTRAINT_NORMAL_BEHAVIOUR(ConstraintType::Distance);

    DistanceConstraint(Object* t1, Object* t2, f32 d, f32 w);
    virtual void solve(f64 dt) override;
    virtual void render(const glm::mat4& proj, mfw::Renderer& renderer) override;

    Object* target[2];
    f32 d, w;
    f64 hardness = 1;
    glm::vec3 color;
    
};
