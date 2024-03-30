#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"

DistanceConstraint::DistanceConstraint(Object* t1, Object* t2, f32 d, f32 w)
    : d(d), w(w), color(COLOR(0xefefef))
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
    renderer.renderCircle(proj, target[0]->m_pos, w, glm::vec4(color, 1));
    renderer.renderRingI(proj, target[0]->m_pos, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_pos, w, glm::vec4(color, 1));
    renderer.renderRingI(proj, target[1]->m_pos, w, glm::vec4(0, 0, 0, 1));

    f64 linew = 0.9;
    renderer.renderLine(proj, target[0]->m_pos, target[1]->m_pos, color, w * linew);

    renderer.renderCircle(proj, target[0]->m_pos, w * 0.2, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_pos, w * 0.2, glm::vec4(0, 0, 0, 1));

    glm::dvec2 ab = target[0]->m_pos - target[1]->m_pos;
    glm::dvec2 normal = glm::normalize(glm::dvec2(-ab.y, ab.x));
    renderer.renderLine(proj, target[0]->m_pos + normal * (f64)w * linew,
                                   target[1]->m_pos + normal * (f64)w * linew, glm::vec3(0), w * (1 - linew));
    renderer.renderLine(proj, target[0]->m_pos - normal * (f64)w * linew,
                                   target[1]->m_pos - normal * (f64)w * linew, glm::vec3(0), w * (1 - linew));
}

