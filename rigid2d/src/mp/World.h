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
public:
    using ObjectContainer = std::vector<RigidBody2D*>;
    using ConstraintContainer = std::vector<Constraint*>;
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
        rigidbodies.push_back(result);
        addObjectToRenderList(result);
        return result;
    }

    template <typename T, typename... Args>
    inline T* addConstraint(const Args& ...args) {
        T* result = new T(args...);
        constraints.push_back(result);
        addObjectToRenderList(result);
        return result;
    }

    template <typename T> [[nodiscard]]
    inline std::vector<T*> findObjects() const {
        std::vector<T*> result{};
        for (auto& object : rigidbodies) {
            if (object->getTypeId() == T::GetTypeId()) {
                result.push_back(static_cast<T*>(object));
            }
        }
        return result;
    }

    template <typename T> [[nodiscard]]
    inline std::vector<T*> getConstraint() const {
        std::vector<T*> result{};
        for (auto& object : constraints) {
            if (object->getTypeId() == T::GetTypeId()) {
                result.push_back(static_cast<T*>(object));
            }
        }
        return result;
    }

    inline void destoryRigidBody(RigidBody2D* body) {
        auto& renderLayer = renderLayers[renderLayersMap[body->getTypeId()]];
        for (u64 j = 0; j < renderLayer.size(); ++j) {
            if (renderLayer[j] == body) {
                renderLayer.erase(renderLayer.begin() + j, renderLayer.begin() + j + 1);
                break;
            }
        }
        for (u64 j = 0; j < rigidbodies.size(); ++j) {
            if (rigidbodies[j] == body) {
                rigidbodies.erase(rigidbodies.begin() + j, rigidbodies.begin() + j + 1);
                delete body;
                break;
            }
        }
    }

private:
    inline void addObjectToRenderList(Object* object) {
        renderLayers[renderLayersMap[object->getTypeId()]].push_back(object);
    }

    void update_physics(const real& dt);
    void update_collision();
    void update_constraint(const real& dt);

    void SweepAndPrune();
    void KDTreesSpacePartitioning();

    ObjectContainer rigidbodies;
    ConstraintContainer constraints;
    std::array<std::vector<Drawable*>, 16> renderLayers;
    std::unordered_map<i32, i32> renderLayersMap;
    static std::unique_ptr<Solver> solver;
    i32 sub_step = 1;


};

