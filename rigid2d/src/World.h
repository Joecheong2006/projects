#pragma once

#include "Mesh.h"
#include "FixPoint.h"
#include "logger.h"

class World {
public:
    glm::vec2 size = glm::vec2(5, 2);

private:
    glm::dvec2 gravity;
    std::vector<Mesh*> meshes;
    std::vector<FixPoint*> fixPoints;

    void wall_collision(f64 dt, Circle* c, const glm::vec2& world);

public:

    explicit World(glm::vec2 size, glm::dvec2 gravity = glm::dvec2(0, -9.81));

    template<typename T>
    inline std::vector<T*>& getObjectContainer() {
        ASSERT(false);
    }

    void update(f64 dt);
    void render(const glm::mat4& proj);
    std::vector<Circle*> findCirclesByPosition(const glm::vec2& pos);
    Circle* findCircleByPosition(const glm::vec2& pos);

};

template<>
inline std::vector<Mesh*>& World::getObjectContainer<Mesh>() {
    return meshes;
}

template<>
inline std::vector<FixPoint*>& World::getObjectContainer<FixPoint>() {
    return fixPoints;
}


