#include "World.h"
#include "../Simulation.h"
#include "EulerOdeSolver.h"
#include "VerletOdeSolver.h"
#include <mfw/mfwlog.h>

std::unique_ptr<OdeSolver> PhysicsWorld::solver = std::make_unique<EulerOdeSolver>();

PhysicsWorld::~PhysicsWorld() {
    clear();
}

void PhysicsWorld::clear() {
    for (auto& objects : objectsContainer) {
        for (auto& object : objects) {
            delete object;
        }
    }
    objectsContainer.clear();
    for (auto& constraints : constraintsContainer) {
        for (auto& constraint : constraints) {
            delete constraint;
        }
    }
    constraintsContainer.clear();
    renderLayersMap.clear();
    objectsTypeMap.clear();
    renderLayers = {};
}

static void apply_friction(RigidBody* obj, vec2 direction, vec2 normal) {
    const real n = abs(glm::dot(Simulation::Get()->world.gravity, normal)) * obj->m_mass;
    obj->addForce(n * 0.03 * glm::clamp(direction, vec2(-1), vec2(1)));
}

static void resolve_velocity(const CollisionState& state, RigidBody* objA, RigidBody* objB) {
    const vec2 tangent = vec2(state.normal.y, -state.normal.x);
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
    if (glm::dot(separate_velocity, state.normal) > 0) {
        return;
    }

    const real e = glm::min(objA->m_restitution, objB->m_restitution);
    // const real e = (objA->m_restitution + objB->m_restitution) * 0.5;
    const real J = -glm::dot(separate_velocity, state.normal) * (e + 1.0) * total_inverse_mass;

    const real a_static = 1.0 - objA->isStatic;
    const real b_static = 1.0 - objB->isStatic;
    objA->m_velocity += a_static * J * objA->m_inverse_mass * state.normal;
    objB->m_velocity -= b_static * J * objB->m_inverse_mass * state.normal;

    return;
    apply_friction(objA, -glm::dot(objA->m_velocity, tangent) * tangent * a_static, state.normal);
    apply_friction(objB, -glm::dot(objB->m_velocity, tangent) * tangent * b_static, state.normal);

    float friction_coefficient = 0.1;
    {
        vec2 tangent_velocity = glm::dot(objA->m_velocity, tangent) * tangent * a_static;
        vec2 normal_velocity = glm::dot(objA->m_velocity, state.normal) * state.normal * a_static;
        if (tangent_velocity.x != 0 || tangent_velocity.y != 0) {
            const real n = -abs(glm::dot(Simulation::Get()->world.gravity, state.normal)) * objA->m_mass;
            objA->addForce(n * friction_coefficient * glm::clamp(tangent_velocity, vec2(-1), vec2(1)));
            const real rotate_direction = glm::normalize(glm::cross(vec3(normal_velocity, 0), vec3(tangent_velocity, 0))).z;
            objA->m_angular_velocity = rotate_direction * glm::length(tangent_velocity) / glm::length(objA->m_position - state.contact);
        }
    }
    {
        vec2 tangent_velocity = glm::dot(objB->m_velocity, tangent) * tangent * b_static;
        vec2 normal_velocity = glm::dot(objB->m_velocity, state.normal) * state.normal * b_static;
        if (tangent_velocity.x != 0 || tangent_velocity.y != 0) {
            const real n = -abs(glm::dot(Simulation::Get()->world.gravity, state.normal)) * objB->m_mass;
            objB->addForce(n * friction_coefficient * glm::clamp(tangent_velocity, vec2(-1), vec2(1)));
            const real rotate_direction = glm::normalize(glm::cross(vec3(normal_velocity, 0), vec3(tangent_velocity, 0))).z;
            objB->m_angular_velocity = rotate_direction * glm::length(tangent_velocity) / glm::length(objB->m_position - state.contact);
        }
    }
}

void PhysicsWorld::update(const real& dt) {
    const real sub_dt = dt / (real)sub_step;
    for (auto& objects : objectsContainer) {
        for (auto& object : objects) {
            object->m_acceleration *= sub_step;
        }
    }
    for (i32 i = 0; i < sub_step; ++i) {
        for (auto& objects : objectsContainer) {
            for (auto& object : objects) {
                object->addForce(gravity * object->m_mass);
            }
        }
        update_collision();
        update_constraint(sub_dt);
        update_physics(sub_dt);
        for (auto& objects : objectsContainer) {
            for (auto& object : objects) {
                object->m_acceleration = {};
                object->m_angular_acceleration = {};
            }
        }
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
    for (auto& objects : objectsContainer) {
        solver->solve(dt, objects);
    }
}

struct Collision {
    RigidBody* body1;
    RigidBody* body2;
    CollisionState state;
};

void PhysicsWorld::update_collision() {
    // std::vector<Collision> collisions;
    // collisions.reserve(100);
    for (auto& object1s : objectsContainer) {
        for (auto& object1 : object1s) {
            for (auto& object2s : objectsContainer) {
                bool is_same = false;
                for (auto& object2 : object2s) {
                    is_same = object1 == object2;
                    if (is_same) 
                        break;
                    if (object1->isStatic and object2->isStatic)
                        continue;
                    if (!object1->collider or !object2->collider)
                        continue;
                    auto state = object1->collider->testCollision(object2->collider, object1, object2);
                    if (state.depth < 0) {
                        // collisions.emplace_back(Collision{object1, object2, state});
                        resolve_velocity(state, object1, object2);
                    }
                }
                if (is_same) 
                    break;
            }
        }
    }
    return;
    // for (auto& collision : collisions) {
    //     resolve_velocity(collision.state, collision.body1, collision.body2);
    // }
}

void PhysicsWorld::update_constraint(const real& dt) {
    for (auto& constraints : constraintsContainer) {
        for (auto& constraint : constraints) {
            constraint->update(dt);
        }
    }
}

