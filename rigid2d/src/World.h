#pragma once

#include "Constraint.h"
#include "RigidBody2D.h"
#include <mfwpch.h>

enum class RenderLayer : i32 {
    Level1,
    Level2,
    Level3,
    Level4,
    Level5,
    Level6,
    Level7,
    Level8,
    Level9,
    Level10,
    Level11,
    Level12,
    Level13,
    Level14,
    Level15,
    Level16,
};

class World {
    using ObjectContainer = std::vector<RigidBody*>;
    using ConstraintContainer = std::vector<Constraint*>;
public:
    glm::vec2 size;
    glm::dvec2 gravity;

    World() = default;
    explicit World(glm::vec2 size, glm::dvec2 gravity = glm::dvec2(0, -9.81));
    ~World();
    
    void clear();
    void update(const f64& dt);
    void render(const glm::mat4& proj, mfw::Renderer& renderer);

    template <typename T>
    inline void setObjectLayer(RenderLayer layer) {
        renderLayersMap[T::GetTypeId()] = (i32)layer;
    }

    template <typename T, typename... Args>
    inline T* addRigidBody(const Args& ...args) {
        T* result = new T(args...);
        i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendObjectsContainer(typeId);
        }
        objectsContainer[objectsTypeMap[typeId]].push_back(result);
        addObjectToRenderList(result);
        return result;
    }

    template <typename T, typename... Args>
    inline T* addConstraint(const Args& ...args) {
        T* result = new T(args...);
        i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendConstraintsContainer(typeId);
        }
        constraintsContainer[objectsTypeMap[typeId]].push_back(result);
        addObjectToRenderList(result);
        return result;
    }

    template <typename T> [[nodiscard]]
    inline ObjectContainer& getObjects() {
        i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendObjectsContainer(typeId);
        }
        return objectsContainer[objectsTypeMap[T::GetTypeId()]];
    }

    template <typename T> [[nodiscard]]
    inline ConstraintContainer& getConstraint() {
        i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendConstraintsContainer(typeId);
        }
        return constraintsContainer[objectsTypeMap[T::GetTypeId()]];
    }

private:
    inline void addObjectToRenderList(Object* object) {
        renderLayers[renderLayersMap[object->getTypeId()]].push_back(object);
    }

    inline void extendObjectsContainer(i32 typeId) {
        objectsTypeMap[typeId] = objectsContainer.size();
        objectsContainer.push_back({});
    }

    inline void extendConstraintsContainer(i32 typeId) {
        objectsTypeMap[typeId] = constraintsContainer.size();
        constraintsContainer.push_back({});
    }

    std::vector<ObjectContainer> objectsContainer;
    std::vector<ConstraintContainer> constraintsContainer;
    std::array<std::vector<Drawable*>, 16> renderLayers;
    std::unordered_map<i32, i32> renderLayersMap;
    std::unordered_map<i32, i32> objectsTypeMap;

};

