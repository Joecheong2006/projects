#include "World.h"
#include <mfw/mfwlog.h>
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

void World::update(const real& dt) {
    for (auto& objects : objectsContainer) {
        for (auto& object : objects) {
            object->addForce(gravity * object->m_mass);
        }
    }
    for (auto& objects : objectsContainer) {
        solver->solve(dt, objects);
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

