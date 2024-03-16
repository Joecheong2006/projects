#include "World.h"
#include "logger.h"

World::World(glm::vec2 size, glm::dvec2 gravity)
    : size(size), gravity(gravity)
{}

World::~World() {
    clear();
}

void World::clear() {
    for (auto& container : objectContainers) {
        for (auto& object : container.second) {
            delete object;
        }
        container.second.clear();
    }
    objectContainers.clear();
    for (auto& container : constraintContainers) {
        for (auto& constraint : container.second) {
            delete constraint;
        }
        container.second.clear();
    }
    constraintContainers.clear();
}

void World::update(const f64& dt) {
    for (auto& container : objectContainers) {
        for (auto& object : container.second) {
            object->addForce(gravity * object->m_mass);
            object->update(dt);
            object->m_acceleration *= 0.6;
        }
    }
    for (auto& containter : constraintContainers) {
        for (auto& constraint : containter.second) {
            constraint->solve(dt);
        }
    }
}

