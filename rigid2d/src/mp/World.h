#include <mfw/mfwpch.h>
#pragma once

#include "Constraint.h"
#include "RigidBody2D.h"
#include "OdeSolver.h"
#include <mfw/mfwlog.h>

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

class PhysicsWorld {
    using ObjectContainer = std::vector<RigidBody*>;
    using ConstraintContainer = std::vector<Constraint*>;
public:
    vec2 gravity = vec2(0, -9.81);

    PhysicsWorld() = default;
    ~PhysicsWorld();
    
    void clear();
    void update(const real& dt);
    void render(const glm::mat4& proj, mfw::Renderer& renderer);
    void setSubStep(i32 step);
    inline i32 getSubStep() { return sub_step; }

    template <typename T>
    inline void setObjectLayer(RenderLayer layer) {
        renderLayersMap[T::GetTypeId()] = (i32)layer;
    }

    template <typename T, typename... Args>
    inline T* addRigidBody(const Args& ...args) {
        T* result = new T(args...);
        const i32 typeId = T::GetTypeId();
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
        const i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendConstraintsContainer(typeId);
        }
        constraintsContainer[objectsTypeMap[typeId]].push_back(result);
        addObjectToRenderList(result);
        return result;
    }

    template <typename T> [[nodiscard]]
    inline ObjectContainer& getObjects() {
        const i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendObjectsContainer(typeId);
        }
        return objectsContainer[objectsTypeMap[typeId]];
    }

    template <typename T> [[nodiscard]]
    inline ConstraintContainer& getConstraint() {
        const i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            extendConstraintsContainer(typeId);
        }
        return constraintsContainer[objectsTypeMap[typeId]];
    }

    template <typename T>
    inline void destoryRigidBody(RigidBody* body) {
        const i32 typeId = T::GetTypeId();
        if (objectsTypeMap.find(typeId) == objectsTypeMap.end()) {
            return;
        }

        auto& objects = objectsContainer[objectsTypeMap[typeId]];
        for (u64 i = 0; i < objects.size(); ++i) {
            if (objects[i] == body) {
                delete objects[i];
                objects.erase(objects.begin() + i, objects.begin() + i + 1);
                auto& renderLayer = renderLayers[renderLayersMap[typeId]];
                renderLayer.erase(renderLayer.begin() + i, renderLayer.begin() + i + 1);
                break;
            }
        }
        return;
        auto& renderLayer = renderLayers[renderLayersMap[typeId]];
        for (u64 i = 0; i < renderLayers.size(); ++i) {
            if (renderLayer[i] == static_cast<Drawable*>(body)) {
                renderLayer.erase(renderLayer.begin() + i, renderLayer.begin() + i + 1);
                break;
            }
        }
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

    void update_physics(const real& dt);
    void update_collision();
    void update_constraint(const real& dt);

    std::vector<ObjectContainer> objectsContainer;
    std::vector<ConstraintContainer> constraintsContainer;
    std::array<std::vector<Drawable*>, 16> renderLayers;
    std::unordered_map<i32, i32> renderLayersMap;
    std::unordered_map<i32, i32> objectsTypeMap;
    static std::unique_ptr<OdeSolver> solver;
    i32 sub_step = 1;

};

