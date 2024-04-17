#include "EulerOdeSolver.h"
#include "RigidBody2D.h"

void EulerOdeSolver::solve(const f64& dt, const std::vector<RigidBody*>& objects) {
    for (auto& object : objects) {
        object->m_oposition = object->m_position;
        object->m_ovelocity = object->m_velocity;

        object->m_velocity += object->m_acceleration * dt;
        object->m_position += object->m_velocity * dt;

        object->m_angular_velocity += object->m_angular_acceleration * dt;
        object->m_angle += object->m_angular_velocity * dt;

        object->m_acceleration = {};
        object->m_angular_acceleration = {};
    }
}

