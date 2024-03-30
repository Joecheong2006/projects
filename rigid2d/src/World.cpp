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
        for (auto& object : container) {
            delete object;
        }
    }
    objectContainers.clear();
    for (auto& container : constraintContainers) {
        for (auto& constraint : container) {
            delete constraint;
        }
    }
    constraintContainers.clear();
}

void World::update(const f64& dt) {
    for (auto& container : objectContainers) {
        for (auto& object : container) {
            object->update(dt);
            object->addForce(gravity * object->m_mass);
        }
    }
    for (auto& containter : constraintContainers) {
        for (auto& constraint : containter) {
            constraint->solve(dt);
        }
    }
}

void World::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    for (auto& containter : constraintContainers) {
        for (auto& constraint : containter) {
            constraint->render(proj, renderer);
        }
    }
    for (auto& container : objectContainers) {
        for (auto& object : container) {
            if (object->display) {
                object->render(proj, renderer);
            }
        }
    }
}

