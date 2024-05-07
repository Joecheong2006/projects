#include "VerletOdeSolver.h"
#include "RigidBody2D.h"

void VerletOdeSolver::solve(const real& dt, const std::vector<RigidBody*>& objects) {
    for (auto& object : objects) {
        if (object->isStatic)
            continue;

        object->m_oposition = object->m_position;
        object->m_ovelocity = object->m_velocity;
        object->m_oacceleration = object->m_acceleration;

        object->m_velocity = object->m_acceleration * dt + glm::pow(1.0 - object->m_damping, dt) * object->m_velocity;
        object->m_position += object->m_velocity * dt + object->m_acceleration * dt * dt * 0.5;

        object->m_angular_velocity += object->m_angular_acceleration * dt;
        object->m_angle += object->m_angular_velocity * dt;
    }
}

