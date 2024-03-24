#include "PointConstraint.h"

#include "Renderer.h"
#include "Circle.h"

PointConstraint::PointConstraint(f32 d, std::function<void(const f64& dt, PointConstraint* pc)> update)
    : onUpdate(update), d(d), target(nullptr), onRender(
            [this](const glm::mat4& proj, mfw::Renderer& renderer, PointConstraint* pc){
                renderer.renderCircle(proj, Circle(self.m_pos, self.m_color, pc->d));
            })
{}

void PointConstraint::solve(f64 dt) {
    onUpdate(dt, this);
}

void PointConstraint::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    onRender(proj, renderer, this);
}

