#pragma once

#include "Constraint.h"
#include "Object.h"
#include <mfwpch.h>

class World {
    using ObjectContainer = std::vector<Object*>;
    using ConstraintContainer = std::vector<Constraint*>;
private:
    std::unordered_map<ObjectType, ObjectContainer> objectContainers;
    std::unordered_map<ConstraintType, ConstraintContainer> constraintContainers;

public:
    glm::vec2 size;
    glm::dvec2 gravity;

    explicit World(glm::vec2 size, glm::dvec2 gravity = glm::dvec2(0, -9.81));

    ~World();
    
    void clear();
    void update(const f64& dt);

    template <typename T>
    inline T* addObject(T* object) {
        objectContainers[T::GetType()].push_back(object);
        return object;
    }

    template <typename T>
    inline T* addConstraint(T* object) {
        constraintContainers[T::GetType()].push_back(object);
        return object;
    }

    template <typename T>
    inline ObjectContainer& getObjects() {
        return objectContainers[T::GetType()];
    }

    template <typename T>
    inline ConstraintContainer& getConstraint() {
        return constraintContainers[T::GetType()];
    }

};

