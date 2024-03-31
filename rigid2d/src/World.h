#pragma once

#include "Constraint.h"
#include "Object.h"
#include <mfwpch.h>
#include <mfwlog.h>

enum class RenderLayer : i32 {
    Level1 = 0,
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
    using ObjectContainer = std::vector<Object*>;
    using ConstraintContainer = std::vector<Constraint*>;
private:
    std::vector<ObjectContainer> objectsContainer;
    std::vector<ConstraintContainer> constraintsContainer;
    std::array<std::vector<Drawable*>, 16> renderLayers;
    std::unordered_map<const char*, i32> renderLayersMap;

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
        renderLayersMap[T::GetTypeName()] = (i32)layer;
    }

    template <typename T, typename... Args>
    inline T* addObject(const Args& ...args) {
        T* result = new T(args...);
        if (T::GetTypeId() + 1 > (i32)objectsContainer.size()) {
            objectsContainer.resize(T::GetTypeId() + 1);
        }
        objectsContainer[T::GetTypeId()].push_back(result);
        renderLayers[renderLayersMap[T::GetTypeName()]].push_back(result);
        return result;
    }

    template <typename T, typename... Args>
    inline T* addConstraint(const Args& ...args) {
        T* result = new T(args...);
        if (T::GetTypeId() + 1 > (i32)constraintsContainer.size()) {
            constraintsContainer.resize(T::GetTypeId() + 1);
        }
        constraintsContainer[T::GetTypeId()].push_back(result);
        renderLayers[renderLayersMap[T::GetTypeName()]].push_back(result);
        return result;
    }

    template <typename T> [[nodiscard]]
    inline ObjectContainer& getObjects() {
        return objectsContainer[T::GetTypeId()];
    }

    template <typename T> [[nodiscard]]
    inline ConstraintContainer& getConstraint() {
        return constraintsContainer[T::GetTypeId()];
         
    }

};

