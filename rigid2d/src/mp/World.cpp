#include "World.h"
#include "EulerOdeSolver.h"
#include "VerletOdeSolver.h"
#include <mfw/mfwlog.h>

std::unique_ptr<OdeSolver> PhysicsWorld::solver = std::make_unique<EulerOdeSolver>();

PhysicsWorld::~PhysicsWorld() {
    clear();
}

void PhysicsWorld::clear() {
    for (auto& object : rigidbodies) {
        delete object;
    }
    rigidbodies.clear();
    for (auto& constraint : constraints) {
        delete constraint;
    }
    constraints.clear();

    renderLayersMap.clear();
    renderLayers = {};
}

static void resolve_velocity(const CollisionState& state, RigidBody* objA, RigidBody* objB) {
    const real total_inverse_mass = 1.0 / (objA->m_inverse_mass + objB->m_inverse_mass);

    if (objB->isStatic) {
        objA->m_position -= state.depth * state.normal;
    }
    else if (objA->isStatic) {
        objB->m_position += state.depth * state.normal;
    }
    else {
        objA->m_position -= state.depth * state.normal * objA->m_inverse_mass * total_inverse_mass;
        objB->m_position += state.depth * state.normal * objB->m_inverse_mass * total_inverse_mass;
    }

    const vec2 separate_velocity = objA->m_velocity - objB->m_velocity;
    const real separate_normal = glm::dot(separate_velocity, state.normal);
    if (separate_normal > 0) {
        return;
    }

    const real e = glm::min(objA->m_restitution, objB->m_restitution);
    const real J = -separate_normal * (e + 1.0) * total_inverse_mass;

    const real a_static = 1.0 - objA->isStatic;
    const real b_static = 1.0 - objB->isStatic;
    objA->m_velocity += a_static * J * objA->m_inverse_mass * state.normal;
    objB->m_velocity -= b_static * J * objB->m_inverse_mass * state.normal;
}

void PhysicsWorld::update(const real& dt) {
    const real sub_dt = dt / (real)sub_step;
    for (auto& object : rigidbodies) {
        object->m_acceleration *= sub_step;
    }
    for (i32 i = 0; i < sub_step; ++i) {
        for (auto& object : rigidbodies) {
            object->addForce(gravity * object->m_mass);
        }
        update_physics(sub_dt);
        update_collision();
        update_constraint(sub_dt);
    }
}

void PhysicsWorld::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    for (auto& layer : renderLayers) {
        for (auto& object : layer) {
            if (object->drawEnable) {
                object->draw(proj, renderer);
            }
        }
    }
}

void PhysicsWorld::setSubStep(i32 step) {
    ASSERT(step > 0);
    sub_step = step;
}

void PhysicsWorld::update_physics(const real& dt) {
    solver->solve(dt, rigidbodies);
}

void PhysicsWorld::update_collision() {
    for (auto& object1 : rigidbodies) {
        for (auto& object2 : rigidbodies) {
            if (object1 == object2) 
                break;
            if (object1->isStatic and object2->isStatic)
                continue;
            if (!object1->collider or !object2->collider)
                continue;
            auto state = object1->collider->testCollision(object2->collider, object1, object2);
            if (state.depth < 0) {
                resolve_velocity(state, object1, object2);
            }
        }
    }
}

void PhysicsWorld::update_constraint(const real& dt) {
    for (auto& constraint : constraints) {
        constraint->update(dt);
    }
}

