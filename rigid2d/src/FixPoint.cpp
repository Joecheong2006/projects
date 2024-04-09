#include "FixPoint.h"
#include "Circle.h"
#include "Simulation.h"

void FixPoint::update([[maybe_unused]] const f64& dt) {
    if (!target)
        return;
    target->m_position = m_position;
    target->m_acceleration = {};
    target->m_velocity = {};
}

void FixPoint::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    Circle(m_position, m_color, d).draw(proj, renderer);
}

FixPoint* ObjectBuilder<FixPoint>::operator()(glm::vec2 pos, f32 d, glm::vec3 color) {
    static const f32 worldScale = Simulation::Get()->getWorldScale();
    auto fixPoint = Simulation::Get()->world.addConstraint<FixPoint>();
    fixPoint->m_position = pos;
    fixPoint->m_color = color;
    fixPoint->d = d * worldScale;
    return fixPoint;
}
FixPoint* ObjectBuilder<FixPoint>::operator()(glm::vec2 pos) {
    return (*this)(pos, default_d, default_color);
}

