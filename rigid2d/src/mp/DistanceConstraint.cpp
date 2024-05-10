#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "glm/gtc/matrix_transform.hpp"

color DistanceConstraint::default_color;
real DistanceConstraint::default_w;

DistanceConstraint::DistanceConstraint(RigidBody2D* target1, RigidBody2D* target2, real d, f32 w, ::color color)
    : d(d), w(w), color(color)
{
    target[0] = target1;
    target[1] = target2;
}

void DistanceConstraint::update(const f64& dt) {
    const real length = glm::length(target[0]->m_position - target[1]->m_position);
    const vec2 direction = (target[0]->m_position - target[1]->m_position) / length;
    const vec2 nd = direction * (d - length) * 0.5;

    const vec2 a = nd / dt / dt;
    if (target[0]->isStatic) {
        target[1]->m_position -= nd * 2.0;
        target[1]->m_acceleration -= a;
    }
    else if (target[1]->isStatic) {
        target[0]->m_position += nd * 2.0;
        target[0]->m_acceleration += a;
    }
    else {
        target[1]->m_position -= nd;
        target[1]->m_acceleration -= a;
        target[0]->m_position += nd;
        target[0]->m_acceleration += a;
    }
}

void DistanceConstraint::draw(const mat4& proj, mfw::Renderer& renderer) {
    // real worldScale = Simulation::Get()->getWorldUnit();

    renderer.renderCircle(proj, target[0]->m_position, w, vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[0]->m_position,
            w * 0.8, vec4(color, 1));
    renderer.renderCircle(proj, target[1]->m_position, w, vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_position,
            w * 0.8, vec4(color, 1));

    renderer.renderLineI(proj, target[0]->m_position, target[1]->m_position, vec3(0), w * 0.97);
    renderer.renderLineI(proj, target[0]->m_position, target[1]->m_position, color,
            w * 0.95);

    renderer.renderCircle(proj, target[0]->m_position, w * 0.3, vec4(0, 0, 0, 1));
    renderer.renderCircle(proj, target[1]->m_position, w * 0.3, vec4(0, 0, 0, 1));
}

