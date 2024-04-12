#include "Spring.h"

#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "Simulation.h"
#include "glm/gtc/matrix_transform.hpp"

Spring::Spring(RigidBody* t1, RigidBody* t2, f32 d, f32 w)
    : DistanceConstraint(t1, t2, d, w)
{
    target[0] = t1;
    target[1] = t2;
}

void Spring::update(const f64& dt) {
    (void)dt;
    f64 current_distance = glm::length(target[0]->m_position - target[1]->m_position);

    const glm::dvec2 direction = glm::normalize(target[0]->m_position - target[1]->m_position);
    const f64 stretch = current_distance - d;
    f64 force_strength = 0;
    force_strength -= stiffness * stretch;
    force_strength -= damping * glm::dot(direction, (target[0]->m_velocity - target[1]->m_velocity));
    glm::dvec2 force = force_strength * direction;

    target[0]->addForce(force);
    target[1]->addForce(-force);
    target[0]->m_position += force * dt * dt / target[0]->m_mass;
    target[1]->m_position -= force * dt * dt / target[1]->m_mass; 
}

#define DRAW_SPRING_SUB_STICK()\
    renderer.renderCircle(proj, Circle(p[1], glm::vec3(0), w));\
    renderer.renderCircle(proj, Circle(p[0], glm::vec3(0), w));\
    renderer.renderCircle(proj, Circle(p[1], m_color, w * 0.6));\
    renderer.renderCircle(proj, Circle(p[0], m_color, w * 0.6));\
    renderer.renderLine(proj, p[1], p[0], glm::vec3(0), w);\
    renderer.renderLine(proj, p[1], p[0], m_color, w * 0.6);

void Spring::draw(const glm::mat4& proj, mfw::Renderer& renderer) {
    const f64 worldScale = Simulation::Get()->getWorldScale();
    const glm::dvec2 pos1 = target[0]->m_position;
    const glm::dvec2 pos2 = target[1]-> m_position;

    const f64 count = 9, len = 0.6 * worldScale;
    const f64 n = glm::length(pos1 - pos2) / (count * worldScale);
    const glm::dvec2 normal = glm::normalize(pos1 - pos2) * worldScale;
    const glm::dvec2 t1 = glm::cross(glm::dvec3(normal, 0), glm::dvec3(0, 0, 1)) * len;
    const glm::dvec2 t2 = glm::cross(glm::dvec3(normal, 0), glm::dvec3(0, 0, -1)) * len;

    glm::dvec2 p[2];
    p[0] = t2 + pos2;
    p[1] = t1 + pos2;
    DRAW_SPRING_SUB_STICK();
    p[1] += normal * n;
    for (i32 i = 0; i < count; i++) {
        DRAW_SPRING_SUB_STICK();
        renderer.renderLine(proj, p[1], p[0], m_color, w * 0.5);
        p[i % 2] += normal * n * 2.0;
    }
    p[0] = t2 + pos1;
    p[1] = t1 + pos1;
    DRAW_SPRING_SUB_STICK();
}


Spring* ObjectBuilder<Spring>::operator()(RigidBody* target1, RigidBody* target2, f32 d, f32 w, glm::vec3 color, f64 stiffness, f64 damping) {
    static const f32 worldScale = Simulation::Get()->getWorldScale();
    auto spring = Simulation::Get()->world.addConstraint<Spring>(target1, target2, d * worldScale, w * worldScale);
    spring->m_color = color;
    spring->stiffness = stiffness;
    spring->damping = damping;
    return spring;
}

Spring* ObjectBuilder<Spring>::operator()(RigidBody* target1, RigidBody* target2, f32 d, f64 stiffness, f64 damping) {
    return (*this)(target1, target2, d, default_w, default_color, stiffness, damping);
}

Spring* ObjectBuilder<Spring>::operator()(RigidBody* target1, RigidBody* target2, f32 d) {
    return (*this)(target1, target2, d, default_w, default_color, default_stiffness, default_damping);
}

