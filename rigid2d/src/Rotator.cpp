#include "Rotator.h"
#include "Simulation.h"
#include "Circle.h"

Rotator::Rotator(Object* center, Object* target): center(center)
{
    this->target = target;
}

void Rotator::solve(const f64& dt) {
    if (!target || !center)
        return;
    target->m_pos += center->m_pos - center->m_opos;
    glm::dvec3 ro = glm::dvec3(glm::normalize(target->m_pos - center->m_pos), 0);
    glm::dvec2 vd = glm::cross(ro, glm::dvec3(0, 0, 1)) * f64(w * r);
    target->m_pos += vd * dt;
    target->m_velocity = vd;
    target->m_acceleration = vd / dt;
    self.m_pos = glm::dvec2(ro) * (f64)r + center->m_pos;
}

// void Rotator::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
//     Circle(self.m_pos, glm::vec3(0), d * 0.2).draw(proj, renderer);
// }

