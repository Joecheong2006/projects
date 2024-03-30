#pragma once

#include "Constraint.h"
#include "Object.h"
#include <mfwpch.h>

class World {
    using ObjectContainer = std::vector<Object*>;
    using ConstraintContainer = std::vector<Constraint*>;
private:
    std::vector<ObjectContainer> objectContainers;
    std::vector<ConstraintContainer> constraintContainers;

public:
    glm::vec2 size;
    glm::dvec2 gravity;

    World() {}
    explicit World(glm::vec2 size, glm::dvec2 gravity = glm::dvec2(0, -9.81));
    ~World();
    
    void clear();
    void update(const f64& dt);
    void render(const glm::mat4& proj, mfw::Renderer& renderer);

    template <typename T, typename... Args>
    inline T* addObject(const Args& ...args) {
        T* result = new T(args...);
        if (T::GetTypeId() + 1 > (i32)objectContainers.size()) {
            objectContainers.resize(T::GetTypeId() + 1);
        }
        objectContainers[T::GetTypeId()].push_back(result);
        return result;
    }

    template <typename T, typename... Args>
    inline T* addConstraint(const Args& ...args) {
        T* result = new T(args...);
        if (T::GetTypeId() + 1 > (i32)constraintContainers.size()) {
            constraintContainers.resize(T::GetTypeId() + 1);
        }
        constraintContainers[T::GetTypeId()].push_back(result);
        return result;
    }

    template <typename T> [[nodiscard]]
    inline ObjectContainer& getObjects() {
        return objectContainers[T::GetTypeId()];
    }

    template <typename T> [[nodiscard]]
    inline ConstraintContainer& getConstraint() {
        return constraintContainers[T::GetTypeId()];
         
    }

};

