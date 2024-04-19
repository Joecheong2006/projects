#pragma once
#include "PointConstraint.h"
#include "ObjectBuilderSet.h"

class Roller: public PointConstraint {
private:
    virtual void update(const real& dt) override;
    virtual void draw(const mat4& proj, mfw::Renderer& renderer) override;

public:
    Roller() = default;
    
};

template <typename T>
struct BuildObject;

template <>
struct BuildObject<Roller> : public ObjectBuilderSet<Roller, Constraint> {
    static color default_color;
    static real default_d;
    BuildObject(vec2 pos, real d = default_d, color color = default_color);
};

