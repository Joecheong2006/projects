#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "Simulation.h"
#include "glm/gtc/matrix_transform.hpp"

DistanceConstraint::DistanceConstraint(RigidBody* t1, RigidBody* t2, f32 d, f32 w)
    : d(d), w(w), m_color()
{
    target[0] = t1;
    target[1] = t2;
}

void DistanceConstraint::update(const f64& dt) {
    (void)dt;
    f64 cd = glm::length(target[0]->m_pos - target[1]->m_pos);
    glm::dvec2 nd = glm::normalize(target[0]->m_pos - target[1]->m_pos) * (d - cd) * 0.5;

    target[0]->m_pos += nd; 
    target[1]->m_pos -= nd; 
#if 1
    glm::dvec2 a = nd / dt / dt;
    // target[0]->addForce(target[0]->m_mass * a);
    // target[1]->addForce(-target[1]->m_mass * a);
    target[0]->m_acceleration += a;
    target[1]->m_acceleration -= a;
#endif
}

void DistanceConstraint::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    f32 worldScale = Simulation::Get()->getWorldScale();

    renderer.renderCircle(proj, target[0]->m_pos, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[0]->m_pos,
            w - worldScale * 0.03, glm::vec4(m_color, 1));
    renderer.renderCircle(proj, target[1]->m_pos, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_pos,
            w - worldScale * 0.03, glm::vec4(m_color, 1));

    renderer.renderLineI(proj, target[0]->m_pos, target[1]->m_pos, glm::vec3(0), w * 0.97);
    renderer.renderLineI(proj, target[0]->m_pos, target[1]->m_pos, m_color,
            w * 0.97 - worldScale * 0.03);

    renderer.renderCircle(proj, target[0]->m_pos, w * 0.3, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_pos, w * 0.3, glm::vec4(0, 0, 0, 1));
}


DistanceConstraint* ObjectBuilder<DistanceConstraint>::operator()(RigidBody* t1, RigidBody* t2, f32 d, f32 w, glm::vec3 color) {
    static const f32 worldScale = Simulation::Get()->getWorldScale();
    auto distanceConstraint = Simulation::Get()->world.addConstraint<DistanceConstraint>(t1, t2, d * worldScale, w * worldScale);
    distanceConstraint->m_color = color;
    return distanceConstraint;
}

DistanceConstraint* ObjectBuilder<DistanceConstraint>::operator()(RigidBody* t1, RigidBody* t2, f32 d) {
    return (*this)(t1, t2, d, default_w, default_color);
}

