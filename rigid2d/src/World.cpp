#include "World.h"
#include "logger.h"

World::World(glm::vec2 size, glm::dvec2 gravity)
    : size(size), gravity(gravity)
{}

World::~World() {
    clear();
}

void World::clear() {
    for (auto& container : objectsContainer) {
        for (auto& object : container) {
            delete object;
        }
    }
    objectsContainer.clear();
    for (auto& container : constraintsContainer) {
        for (auto& constraint : container) {
            delete constraint;
        }
    }
    constraintsContainer.clear();
    renderLayersMap.clear();
    renderLayers = {};
}

void World::update(const f64& dt) {
    for (auto& container : objectsContainer) {
        for (auto& object : container) {
            object->update(dt);
            object->addForce(gravity * object->m_mass);
        }
    }
    for (auto& containter : constraintsContainer) {
        for (auto& constraint : containter) {
            constraint->solve(dt);
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

