#pragma once
#include "PointConstraint.h"
#include "ObjectBuilderSet.h"

class FixPoint: public PointConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    FixPoint() = default;
    
};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<FixPoint> : public ObjectBuilderSet<FixPoint, Constraint> {
    static color default_color;
    static real default_d;
    BuildObject(vec2 position, real d = default_d, color color = default_color);
};

