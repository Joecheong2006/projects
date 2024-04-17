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
    const real length = glm::length(target[0]->m_position - target[1]->m_position);
    const vec2 direction = glm::normalize(target[0]->m_position - target[1]->m_position);
    const vec2 nd = direction * (d - length) * 0.5;

    // target[0]->m_position += nd; 
    // target[1]->m_position -= nd; 

    const vec2 a = nd / dt / dt;
    target[0]->m_acceleration += a;
    target[1]->m_acceleration -= a;
}

void DistanceConstraint::draw(const mat4& proj, mfw::Renderer& renderer) {
    real worldScale = Simulation::Get()->getWorldScale();

    renderer.renderCircle(proj, target[0]->m_position, w, vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[0]->m_position,
            w - worldScale * 0.03, vec4(color, 1));
    renderer.renderCircle(proj, target[1]->m_position, w, vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_position,
            w - worldScale * 0.03, vec4(color, 1));

    renderer.renderLineI(proj, target[0]->m_position, target[1]->m_position, vec3(0), w * 0.97);
    renderer.renderLineI(proj, target[0]->m_position, target[1]->m_position, color,
            w * 0.97 - worldScale * 0.03);

    renderer.renderCircle(proj, target[0]->m_position, w * 0.3, vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_position, w * 0.3, vec4(0, 0, 0, 1));
}

color ObjectBuilder<DistanceConstraint>::default_color;
real ObjectBuilder<DistanceConstraint>::default_w;

DistanceConstraint* ObjectBuilder<DistanceConstraint>::operator()(RigidBody* target1, RigidBody* target2, real d, f32 w, color color) {
    const real worldScale = Simulation::Get()->getWorldScale();
    auto distanceConstraint = Simulation::Get()->world.addConstraint<DistanceConstraint>(target1, target2, d * worldScale, w * worldScale);
    distanceConstraint->color = color;
    return distanceConstraint;
}

