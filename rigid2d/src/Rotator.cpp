#include "Rotator.h"
#include "Simulation.h"
#include "Circle.h"
#include <mfw/mfwlog.h>

Rotator::Rotator(RigidBody* center, RigidBody* target)
    : center(center), target(target)
{}

void Rotator::update(const real& dt) {
    if (!target || !center)
        return;
    target->m_position += center->m_position - center->m_oposition;
    vec3 ro = vec3(glm::normalize(target->m_position - center->m_position), 0);
    vec2 vd = glm::cross(ro, vec3(0, 0, 1)) * w * r;
    target->m_position += vd * dt;
    target->m_velocity = vd;
    target->m_acceleration = vd / dt;
    m_pos = vec2(ro) * (f64)r + center->m_position;
}

void Rotator::draw(const mat4& proj, mfw::Renderer& renderer) {
    TOVOID(proj, renderer);
};

real ObjectBuilder<Rotator>::default_r;
real ObjectBuilder<Rotator>::default_w;

Rotator* ObjectBuilder<Rotator>::operator()(RigidBody* center, RigidBody* target, real r, real w) {
    const real worldScale = Simulation::Get()->getWorldScale();
    auto rotator = Simulation::Get()->world.addConstraint<Rotator>(center, target);
    rotator->r = r * worldScale;
    rotator->w = w;
    return rotator;
}

