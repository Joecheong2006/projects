#include "Spring.h"

#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "Simulation.h"
#include "glm/gtc/matrix_transform.hpp"

Spring::Spring(RigidBody* t1, RigidBody* t2, real d, real w)
    : DistanceConstraint(t1, t2, d, w)
{
    target[0] = t1;
    target[1] = t2;
}

void Spring::update(const real& dt) {
    (void)dt;
    real current_distance = glm::length(target[0]->m_position - target[1]->m_position);

    const vec2 direction = glm::normalize(target[0]->m_position - target[1]->m_position);
    const real stretch = current_distance - d;
    real force_strength = 0;
    force_strength -= stiffness * stretch;
    force_strength -= damping * glm::dot(direction, target[0]->m_velocity - target[1]->m_velocity);
    vec2 force = force_strength * direction;

    target[0]->addForce(force);
    target[1]->addForce(-force);
    // target[0]->m_position += force * dt * dt / target[0]->m_mass;
    // target[1]->m_position -= force * dt * dt / target[1]->m_mass; 
}

#define DRAW_SPRING_SUB_STICK()\
    renderer.renderCircle(proj, Circle(p[1], glm::vec3(0), w));\
    renderer.renderCircle(proj, Circle(p[0], glm::vec3(0), w));\
    renderer.renderCircle(proj, Circle(p[1], color, w * 0.6));\
    renderer.renderCircle(proj, Circle(p[0], color, w * 0.6));\
    renderer.renderLine(proj, p[1], p[0], glm::vec3(0), w);\
    renderer.renderLine(proj, p[1], p[0], color, w * 0.6);

void Spring::draw(const mat4& proj, mfw::Renderer& renderer) {
    const real worldScale = Simulation::Get()->getWorldScale();
    const vec2 pos1 = target[0]->m_position;
    const vec2 pos2 = target[1]-> m_position;

    //const real count = i32(d * 12), len = 0.6 * worldScale;
    const real len = 0.6 * worldScale;
    const real n = glm::length(pos1 - pos2) / (count * worldScale);
    const vec2 normal = glm::normalize(pos1 - pos2) * worldScale;
    const vec2 t1 = glm::cross(vec3(normal, 0), vec3(0, 0, 1)) * len;
    const vec2 t2 = glm::cross(vec3(normal, 0), vec3(0, 0, -1)) * len;

    vec2 p[2];
    p[0] = t2 + pos2;
    p[1] = t1 + pos2;
    DRAW_SPRING_SUB_STICK();
    p[1] += normal * n;
    for (i32 i = 0; i < count; i++) {
        DRAW_SPRING_SUB_STICK();
        renderer.renderLine(proj, p[1], p[0], color, w * 0.5);
        p[i % 2] += normal * n * 2.0;
    }
    p[0] = t2 + pos1;
    p[1] = t1 + pos1;
    DRAW_SPRING_SUB_STICK();
}

color BuildObject<Spring>::default_color;
real BuildObject<Spring>::default_w;
real BuildObject<Spring>::default_stiffness;
real BuildObject<Spring>::default_damping;
i32 BuildObject<Spring>::default_count = 1;

BuildObject<Spring>::BuildObject(
        RigidBody* target1, RigidBody* target2,
        real d, real stiffness, real damping,
        i32 count, real w,color color) {
    const real worldScale = Simulation::Get()->getWorldScale();
    object = Simulation::Get()->world.addConstraint<Spring>(target1, target2, d * worldScale, w * worldScale);
    object->color = color;
    object->stiffness = stiffness;
    object->damping = damping;
    object->count = count;
}

