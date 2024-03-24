#include "DistanceConstraint.h"
#include "Renderer.h"

DistanceConstraint::DistanceConstraint(Object* t1, Object* t2, f32 d, f32 w)
    : d(d), w(w), color(0.95)
{
    target[0] = t1;
    target[1] = t2;
}

void DistanceConstraint::solve(f64 dt) {
    (void)dt;
    f64 cd = glm::length(target[0]->m_pos - target[1]->m_pos);
    glm::dvec2 nd = glm::normalize(target[0]->m_pos - target[1]->m_pos) * (d - cd) * 0.5 * hardness;

    target[0]->m_pos += nd; 
    target[1]->m_pos -= nd; 
#if 1
    //target[0]->m_velocity += (nd / dt);
    //target[1]->m_velocity -= (nd / dt);
    target[0]->m_acceleration += (nd / dt) / dt;
    target[1]->m_acceleration -= (nd / dt) / dt;
#endif
}

void DistanceConstraint::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    renderer.renderRactangle(proj, target[0]->m_pos, target[1]->m_pos, color, w);
}

