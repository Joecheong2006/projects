#include "PointConstraint.h"

#include "Renderer.h"
#include "Circle.h"
#include "mfwlog.h"

PointConstraint::PointConstraint(f32 d, std::function<void(const f64& dt, PointConstraint* pc)> update)
    : onUpdate(update), d(d), target(nullptr)
    , onRender([](const glm::mat4& proj, mfw::Renderer& renderer, PointConstraint* pc){
                TOVOID(proj, renderer, pc);
            })
{}

void PointConstraint::solve(f64 dt) {
    onUpdate(dt, this);
}

void PointConstraint::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    onRender(proj, renderer, this);
}

