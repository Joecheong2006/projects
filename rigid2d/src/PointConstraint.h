#pragma once

#include "Constraint.h"
#include "Circle.h"
#include "util.h"
#include <mfwpch.h>

namespace mfw {
    class Renderer;
};

class PointConstraint: public Constraint {
private:
    std::function<void(const f64& dt, PointConstraint* pc)> update;

public:
    SET_CONSTRAINT_NORMAL_BEHAVIOUR(ConstraintType::Point);

    PointConstraint(f32 d, std::function<void(const f64& dt, PointConstraint* pc)> update);

    virtual void solve(f64 dt) override;
    void render(const glm::mat4& proj, mfw::Renderer& renderer) const;

    f32 d;
    Object* target;
    Object self;
    
};

