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
    }
    objectContainers = {};
    for (auto& container : constraintContainers) {
        for (auto& constraint : container.second) {
            delete constraint;
        }
    }
    constraintContainers = {};
}

void World::update(const f64& dt) {
    for (auto& container : objectContainers) {
        for (auto& object : container.second) {
            object->update(dt);
            object->addForce(gravity * object->m_mass);
        }
    }
    for (auto& containter : constraintContainers) {
        for (auto& constraint : containter.second) {
            constraint->solve(dt);
        }
    }
}

void World::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    for (auto& containter : constraintContainers) {
        for (auto& constraint : containter.second) {
            constraint->render(proj, renderer);
        }
    }
    for (auto& container : objectContainers) {
        for (auto& object : container.second) {
            object->render(proj, renderer);
        }
    }
}

