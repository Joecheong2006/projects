#include "PointConstraint.h"
#include "Renderer.h"

PointConstraint::PointConstraint(f32 d, std::function<void(const f64& dt, PointConstraint* pc)> update)
    : update(update), d(d), target(nullptr)
{}

void PointConstraint::solve(f64 dt) {
    update(dt, this);
}

void PointConstraint::render(const glm::mat4& proj, mfw::Renderer& renderer) const {
    renderer.renderCircle(proj, Circle(self.m_pos, self.m_color, d));
}

