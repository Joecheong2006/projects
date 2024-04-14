#include "EulerOdeSolver.h"
#include "RigidBody2D.h"

void EulerOdeSolver::solve(const f64& dt, const std::vector<RigidBody*>& objects) {
    for (auto& object : objects) {
        object->m_opos = object->m_position;
        object->m_ovelocity = object->m_velocity;
        object->m_velocity += object->m_acceleration * dt;
        object->m_position += object->m_velocity * dt;
        object->m_acceleration = {};
    }
}

