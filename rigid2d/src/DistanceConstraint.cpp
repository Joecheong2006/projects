#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "Simulation.h"

DistanceConstraint::DistanceConstraint(Object* t1, Object* t2, f32 d, f32 w)
    : d(d), w(w), color(COLOR(0xefefef))
{
    target[0] = t1;
    target[1] = t2;
}

void DistanceConstraint::solve(const f64& dt) {
    (void)dt;
    f64 cd = glm::length(target[0]->m_pos - target[1]->m_pos);
    glm::dvec2 nd = glm::normalize(target[0]->m_pos - target[1]->m_pos) * (d - cd) * 0.5 * hardness;

    target[0]->m_pos += nd; 
    target[1]->m_pos -= nd; 
#if 1
    target[0]->m_acceleration += nd / dt / dt;
    target[1]->m_acceleration -= nd / dt / dt;
#endif
}

void DistanceConstraint::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    f32 worldScale = Simulation::Get()->getWorldScale();

    renderer.renderCircleI(proj, target[0]->m_pos, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircleI(proj, target[0]->m_pos,
            w - worldScale * 0.03, glm::vec4(color, 1));
    renderer.renderCircleI(proj, target[1]->m_pos, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircleI(proj, target[1]->m_pos,
            w - worldScale * 0.03, glm::vec4(color, 1));

    renderer.renderLineI(proj, target[0]->m_pos, target[1]->m_pos, glm::vec3(0), w);
    renderer.renderLineI(proj, target[0]->m_pos, target[1]->m_pos, color,
            w - worldScale * 0.03);

    renderer.renderCircleI(proj, target[0]->m_pos, w * 0.3, glm::vec4(0, 0, 0, 1));
    renderer.renderCircleI(proj, target[1]->m_pos, w * 0.3, glm::vec4(0, 0, 0, 1));
}

