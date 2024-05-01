#include "World.h"
#include "Simulation.h"
#include "EulerOdeSolver.h"

void World::initialize(glm::dvec2 gravity) {
    this->gravity = gravity;
    solver = std::make_unique<EulerOdeSolver>();
}

World::~World() {
    clear();
}

void World::clear() {
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
    const vec2 normal = state.normal;
    const vec2 tangent = vec2(normal.y, -normal.x);
    const real total_inverse_mass = 1.0 / (objA->m_inverse_mass + objB->m_inverse_mass);

    if (objB->isStatic) {
        objA->m_position -= state.depth * normal;
    }
    else if (objA->isStatic) {
        objB->m_position += state.depth * normal;
    }
    else {
        objA->m_position -= state.depth * normal * objA->m_inverse_mass * total_inverse_mass;
        objB->m_position += state.depth * normal * objB->m_inverse_mass * total_inverse_mass;
    }

    const vec2 separate_velocity = objA->m_velocity - objB->m_velocity;
    if (glm::dot(separate_velocity, normal) > 0) {
        return;
    }
    const real e = glm::min(objA->m_restitution, objB->m_restitution);
    const real J = -glm::dot(separate_velocity, normal) * (e + 1.0) * total_inverse_mass;

    const real a_static = 1.0 - objA->isStatic;
    const real b_static = 1.0 - objB->isStatic;
    objA->m_velocity += a_static * J * objA->m_inverse_mass * normal;
    objB->m_velocity -= b_static * J * objB->m_inverse_mass * normal;

    apply_friction(objA, -glm::dot(objA->m_velocity, tangent) * tangent * a_static, normal);
    apply_friction(objB, -glm::dot(objB->m_velocity, tangent) * tangent * b_static, normal);

    return;
    float friction_coefficient = 0.1;
    {
        vec2 tangent_velocity = glm::dot(objA->m_velocity, tangent) * tangent * a_static;
        vec2 normal_velocity = glm::dot(objA->m_velocity, normal) * normal * a_static;
        if (tangent_velocity.x != 0 || tangent_velocity.y != 0) {
            const real n = -abs(glm::dot(Simulation::Get()->world.gravity, normal)) * objA->m_mass;
            objA->addForce(n * friction_coefficient * glm::clamp(tangent_velocity, vec2(-1), vec2(1)));
            const real rotate_direction = glm::normalize(glm::cross(vec3(normal_velocity, 0), vec3(tangent_velocity, 0))).z;
            objA->m_angular_velocity = rotate_direction * glm::length(tangent_velocity) / glm::length(objA->m_position - state.contact);
        }
    }
    {
        vec2 tangent_velocity = glm::dot(objB->m_velocity, tangent) * tangent * b_static;
        vec2 normal_velocity = glm::dot(objB->m_velocity, normal) * normal * b_static;
        if (tangent_velocity.x != 0 || tangent_velocity.y != 0) {
            const real n = -abs(glm::dot(Simulation::Get()->world.gravity, normal)) * objB->m_mass;
            objB->addForce(n * friction_coefficient * glm::clamp(tangent_velocity, vec2(-1), vec2(1)));
            const real rotate_direction = glm::normalize(glm::cross(vec3(normal_velocity, 0), vec3(tangent_velocity, 0))).z;
            objB->m_angular_velocity = rotate_direction * glm::length(tangent_velocity) / glm::length(objB->m_position - state.contact);
        }
    }
}

void World::update(const real& dt) {
    for (auto& objects : objectsContainer) {
        for (auto& object : objects) {
            object->addForce(gravity * object->m_mass);
        }
    }
    for (auto& objects : objectsContainer) {
        solver->solve(dt, objects);
    }

    for (auto& object1s : objectsContainer) {
        for (auto& object1 : object1s) {
            for (auto& object2s : objectsContainer) {
                for (auto& object2 : object2s) {
                    if (object1 == object2)
                        break;
                    if (object1->isStatic && object2->isStatic)
                        continue;
                    auto state = object1->collider->testCollision(object2->collider, object1, object2);
                    if (state.depth < 0) {
                        resolve_velocity(state, object1, object2);
                    }
                }
            }
        }
    }

    for (auto& constraints : constraintsContainer) {
        for (auto& constraint : constraints) {
            constraint->update(dt);
        }
    }
}

void World::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    for (auto& layer : renderLayers) {
        for (auto& object : layer) {
            if (object->drawEnable) {
                object->draw(proj, renderer);
            }
        }
    }
}

