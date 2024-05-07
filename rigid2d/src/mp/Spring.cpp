#include "Spring.h"
#include "DistanceConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "../Simulation.h"
#include "glm/gtc/matrix_transform.hpp"

color Spring::default_color;
real Spring::default_w;
real Spring::default_stiffness;
real Spring::default_damping;
i32 Spring::default_count = 1;

Spring::Spring(
        RigidBody* target1, RigidBody* target2,
        real d, real stiffness, real damping,
        i32 count, real w, ::color color)
    : DistanceConstraint(target1, target2, d, w)
{
    this->color = color;
    this->stiffness = stiffness;
    this->damping = damping;
    this->count = count;
}

void Spring::update(const real& dt) {
    (void)dt;
    const real current_distance = glm::length(target[0]->m_position - target[1]->m_position);

    const vec2 direction = (target[0]->m_position - target[1]->m_position) / current_distance;
    const real stretch = current_distance - d;
    real force_strength = 0;
    force_strength -= stiffness * stretch;
    force_strength -= damping * glm::dot(direction, target[0]->m_velocity - target[1]->m_velocity);
    const vec2 force = force_strength * direction;

    target[0]->addForce(force * (1.0 - target[0]->isStatic));
    target[1]->addForce(-force * (1.0 - target[1]->isStatic));
}

#define DRAW_SPRING_SUB_STICK()\
    renderer.renderCircle(proj, Circle(p[1], w, glm::vec3(0)));\
    renderer.renderCircle(proj, Circle(p[0], w, glm::vec3(0)));\
    renderer.renderCircle(proj, Circle(p[1], w * 0.6, color));\
    renderer.renderCircle(proj, Circle(p[0], w * 0.6, color));\
    renderer.renderLine(proj, p[1], p[0], glm::vec3(0), w);\
    renderer.renderLine(proj, p[1], p[0], color, w * 0.6);

void Spring::draw(const mat4& proj, mfw::Renderer& renderer) {
    const real worldScale = Simulation::Get()->getWorldUnit();
    const vec2 pos1 = target[0]->m_position;
    const vec2 pos2 = target[1]-> m_position;

    //const real count = i32(d * 12), len = 0.6 * worldScale;
    const real len = 0.6 * worldScale;
    const real n = glm::length(pos1 - pos2) / (count * worldScale);
    const vec2 normal = glm::normalize(pos1 - pos2) * worldScale;
    const vec2 t1 = glm::cross(vec3(normal, 0), vec3(0, 0, 1)) * len;
    const vec2 t2 = glm::cross(vec3(normal, 0), vec3(0, 0, -1)) * len;

    renderer.renderLine(proj, pos1, pos2, color, w * 0.5);
    return;
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

