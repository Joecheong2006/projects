#include "Rotator.h"
#include "Circle.h"
#include <mfw/mfwlog.h>

real Rotator::default_r;
real Rotator::default_w;

Rotator::Rotator(RigidBody2D* center, RigidBody2D* target, real r, real w)
    : r(r), w(w), center(center), target(target)
{
}

void Rotator::update(const real& dt) {
    if (!target || !center)
        return;
    // target->m_position += center->m_position - center->m_oposition;
    target->m_position += center->m_velocity * dt;
    const vec3 ro = vec3(glm::normalize(target->m_position - center->m_position), 0);
    const vec2 vd = glm::cross(ro, vec3(0, 0, 1)) * w * r;
    target->m_velocity = vd;
    m_pos = vec2(ro) * r + center->m_position;
}

void Rotator::draw(const mat4& proj, mfw::Renderer& renderer) {
    TOVOID(proj, renderer);
};

