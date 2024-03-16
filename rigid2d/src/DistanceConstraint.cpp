#include "DistanceConstraint.h"
#include "Renderer.h"

DistanceConstraint::DistanceConstraint(Object* t1, Object* t2, f32 d)
    : d(d)
{
    target[0] = t1;
    target[1] = t2;
}

void DistanceConstraint::solve(f64 dt) {
    (void)dt;
    f64 cd = glm::length(target[0]->m_pos - target[1]->m_pos);
    glm::dvec2 nd = glm::normalize(target[0]->m_pos - target[1]->m_pos) * (d - cd) * 0.5;

    target[0]->m_pos += nd; 
    target[1]->m_pos -= nd; 
}

void DistanceConstraint::render(const glm::mat4& proj, mfw::Renderer& renderer, f32 w) const {
    renderer.renderRactangle(proj, target[0]->m_pos, target[1]->m_pos, glm::vec3(1), w);
}

