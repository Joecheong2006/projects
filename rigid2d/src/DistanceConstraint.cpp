#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "Simulation.h"
#include "glm/gtc/matrix_transform.hpp"

DistanceConstraint::DistanceConstraint(RigidBody* t1, RigidBody* t2, real d, real w)
    : d(d), w(w), color()
{
    target[0] = t1;
    target[1] = t2;
}

void DistanceConstraint::update(const f64& dt) {
    (void)dt;
    f64 cd = glm::length(target[0]->m_position - target[1]->m_position);
    glm::dvec2 nd = glm::normalize(target[0]->m_position - target[1]->m_position) * (d - cd) * 0.5;

    target[0]->m_position += nd; 
    target[1]->m_position -= nd; 
#if 1
    glm::dvec2 a = nd / dt / dt;
    target[0]->m_acceleration += a;
    target[1]->m_acceleration -= a;
#endif
}

void DistanceConstraint::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    f32 worldScale = Simulation::Get()->getWorldScale();

    renderer.renderCircle(proj, target[0]->m_position, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[0]->m_position,
            w - worldScale * 0.03, glm::vec4(color, 1));
    renderer.renderCircle(proj, target[1]->m_position, w, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_position,
            w - worldScale * 0.03, glm::vec4(color, 1));

    renderer.renderLineI(proj, target[0]->m_position, target[1]->m_position, glm::vec3(0), w * 0.97);
    renderer.renderLineI(proj, target[0]->m_position, target[1]->m_position, color,
            w * 0.97 - worldScale * 0.03);

    renderer.renderCircle(proj, target[0]->m_position, w * 0.3, glm::vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_position, w * 0.3, glm::vec4(0, 0, 0, 1));
}

color ObjectBuilder<DistanceConstraint>::default_color;
real ObjectBuilder<DistanceConstraint>::default_w;

DistanceConstraint* ObjectBuilder<DistanceConstraint>::operator()(RigidBody* target1, RigidBody* target2, real d, f32 w, color color) {
    const real worldScale = Simulation::Get()->getWorldScale();
    auto distanceConstraint = Simulation::Get()->world.addConstraint<DistanceConstraint>(target1, target2, d * worldScale, w * worldScale);
    distanceConstraint->color = color;
    return distanceConstraint;
}

