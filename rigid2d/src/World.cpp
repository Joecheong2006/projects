#include "World.h"

World::World(glm::vec2 size, glm::dvec2 gravity)
    : size(size), gravity(gravity)
{}

void World::update(f64 dt) {
    for (auto& mesh : meshes) {
        for (auto& e : mesh->entities) {
            e->addForce(gravity * e->m_mass);
            e->update(dt);
            e->m_acceleration *= 0.4;
            wall_collision(dt, e, size);
        }
        for (auto& stick : mesh->sticks) {
            stick->update(dt);
        }
    }
    for (auto& point : fixPoints)  {
        point->fix();
    }
}

void World::wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
    if (c->m_pos.y - c->r < -world.y) {
        f64 ay = ((-world.y - c->m_pos.y + c->r) - (c->m_pos.y - c->m_opos.y)) / (dt * dt);
        c->addForce(glm::dvec2(0, ay * c->m_mass));
    }
    if (c->m_pos.x - c->r < -world.x) {
        f64 ax = ((-world.x - c->m_pos.x + c->r) - (c->m_pos.x - c->m_opos.x)) / (dt * dt);
        c->addForce(glm::dvec2(ax * c->m_mass, 0));
    }
    else if (c->m_pos.x + c->r > world.x) {
        f64 ax = ((world.x - c->m_pos.x - c->r) - (c->m_pos.x - c->m_opos.x)) / (dt * dt);
        c->addForce(glm::dvec2(ax * c->m_mass, 0));
    }
};

void World::render(const glm::mat4& proj) {
    for (auto& fixPoint : fixPoints) {
        fixPoint->render(proj);
    }
    for (auto& mesh : meshes) {
        for (auto& stick : mesh->sticks) {
            stick->render(proj);
        }
        for (auto& e : mesh->entities) {
            Circle::renderer->draw(proj, *e);
        }
    }
}

std::vector<Circle*> World::findCirclesByPosition(const glm::vec2& pos) {
    std::vector<Circle*> result;
    for (auto& mesh : meshes) {
        for (auto& e : mesh->entities) {
            if (e->r > glm::length((const glm::vec2&)e->m_pos - pos)) {
                result.push_back(e);
            }
        }
    }
    return result;
}

Circle* World::findCircleByPosition(const glm::vec2& pos) {
    for (auto& mesh : meshes) {
        for (auto& e : mesh->entities) {
            if (e->r > glm::length((const glm::vec2&)e->m_pos - pos)) {
                return e;
            }
        }
    }
    return nullptr;
}


