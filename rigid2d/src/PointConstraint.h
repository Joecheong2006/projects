#pragma once

#include "Constraint.h"
#include "Object.h"
#include "util.h"
#include "mfwpch.h"

namespace mfw {
    class Renderer;
};

class PointConstraint: public Constraint {
private:
    std::function<void(const f64& dt, PointConstraint* pc)> onUpdate;

public:
    GENERATE_CONSTRAINT_IDENTIFIER();

    PointConstraint(f32 d, std::function<void(const f64& dt, PointConstraint* pc)> update);

    virtual void solve(f64 dt) override;
    virtual void render(const glm::mat4& proj, mfw::Renderer& renderer) override;

    f32 d;
    Object* target;
    Object self;
    std::function<void(const glm::mat4& proj, mfw::Renderer& renderer, PointConstraint* pc)> onRender;
    
};

