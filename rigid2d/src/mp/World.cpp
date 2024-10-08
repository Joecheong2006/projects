#include "World.h"
#include "EulerOdeSolver.h"
#include "VerletOdeSolver.h"
#include "TestCollision2D.h"
#include <mfw/mfwlog.h>

#include <mutex>
#include <algorithm>
#include <execution>

std::unique_ptr<Solver> PhysicsWorld::solver = std::make_unique<EulerOdeSolver>();
// std::unique_ptr<Solver> PhysicsWorld::solver = std::make_unique<VerletOdeSolver>();

PhysicsWorld::~PhysicsWorld() {
    clear();
}

void PhysicsWorld::clear() {
    for (auto& object : rigidbodies) {
        delete object;
    }
    rigidbodies.clear();
    for (auto& constraint : constraints) {
        delete constraint;
    }
    constraints.clear();

    renderLayersMap.clear();
    renderLayers = {};
}

static void resolve_velocity(const CollisionState& state, RigidBody2D* objA, RigidBody2D* objB) {
    const real total_inverse_mass = 1.0 / (objA->getInverseMass() + objB->getInverseMass());

    if (objB->isStatic) {
        objA->m_position -= state.depth * state.normal;
    }
    else if (objA->isStatic) {
        objB->m_position += state.depth * state.normal;
    }
    else {
        objA->m_position -= state.depth * state.normal * objA->getInverseMass() * total_inverse_mass;
        objB->m_position += state.depth * state.normal * objB->getInverseMass() * total_inverse_mass;
    }

    const real separate_normal = glm::dot(objA->m_velocity - objB->m_velocity, state.normal);
    if (separate_normal > 0) {
        return;
    }

    const real e = glm::min(objA->m_restitution, objB->m_restitution);
    const real J = -separate_normal * (e + 1.0) * total_inverse_mass;

    const real a_static = 1.0 - objA->isStatic;
    const real b_static = 1.0 - objB->isStatic;
    objA->m_velocity += a_static * J * objA->getInverseMass() * state.normal;
    objB->m_velocity -= b_static * J * objB->getInverseMass() * state.normal;
}

#define POSITION_TO_LOCATION(pos, block_size)\
        ((i64(0) | i32((pos).x / block_size)) << 32) | i32((pos).y / block_size)

#define VEC2_TO_LOCATION(vec)\
        POSITION_TO_LOCATION(vec, 1);

#define LOCATION_TO_VEC2(location)\
        glm::ivec2(i32(location >> 32), i32(location & 0xffffffff00000000 >> 32))

template <i32 width, i32 height>
class FixGrid {
public:
    FixGrid() = default;

    std::array<PhysicsWorld::ObjectContainer, width * height> location_map{};
    real block_size;

    void update(PhysicsWorld::ObjectContainer& bodies) {
        for (i32 i = 0; i < height; ++i) {
            for (i32 j = 0; j < width; ++j) {
                glm::vec2 center = vec2(j, i) * block_size + vec2(block_size) * 0.5 - vec2(width, height) * 0.5 * block_size;
                AABB cell = AABB(vec2(block_size), center);
                for (auto& body : bodies) {
                    if (TestCollision2D::AABB(body->getCollider().getBounds(), cell)) {
                        location_map[j + i * width].push_back(body);
                    }
                }
            }
        }
        return;
        for (auto& body : bodies) {
            const glm::ivec2 location = (body->m_position + vec2(width, height) * 0.5 * block_size) / block_size;
            const i32 key = location.x + location.y * width;
            if (0 > key or key > width * height - 1) {
                continue;
            }
            location_map[key].push_back(body);
        }
    }

    void solve_pair(PhysicsWorld::ObjectContainer& pair1, PhysicsWorld::ObjectContainer& pair2) {
        for (auto& object1 : pair1) {
            for (auto& object2 : pair2) {
                if (object1 == object2) 
                    break;
                if (object1->isStatic and object2->isStatic)
                    continue;
                auto state = object1->getCollider().testCollision(&object2->getCollider(), object1, object2);
                if (state.depth < 0) {
                    resolve_velocity(state, object1, object2);
                }
            }
        }
    }

    void check_neibor(std::vector<RigidBody2D *>& current, glm::ivec2 index) {
        for (i32 i = -1; i <= 1; ++i) {
            for (i32 j = -1; j <= 1; ++j) {
                const glm::ivec2 location = index + glm::ivec2(j, i);
                const i64 neibor_key = location.x + location.y * width;
                solve_pair(location_map[neibor_key], current);
            }
        }
    }

    void solve() {
        for (i32 i = 1; i < height - 1; ++i) {
            for (i32 j = 1; j < width - 1; ++j) {
                check_neibor(location_map[j + i * width], {j, i});
            }
        }
    }

};

void PhysicsWorld::update(const real& dt) {
    const real sub_dt = dt / sub_step;

    for (auto& object : rigidbodies) {
        object->addForce(gravity * object->getMass());
        object->m_acceleration *= sub_step;
    }
    
    for (i32 i = 0; i < sub_step; ++i) {
        update_physics(sub_dt);
        update_collision();
        // {
        //     FixGrid<20, 20> grid;
        //     grid.block_size = 1.0_mu;
        //     grid.update(rigidbodies);
        //     grid.solve();
        // }
        update_constraint(sub_dt);
    }
}

void PhysicsWorld::render(const glm::mat4& proj, mfw::Renderer& renderer) {
    for (auto& layer : renderLayers) {
        for (auto& object : layer) {
            if (object->drawEnable) {
                object->draw(proj, renderer);
            }
        }
    }
}

void PhysicsWorld::setSubStep(i32 step) {
    ASSERT(step > 0);
    sub_step = step;
}

void PhysicsWorld::update_physics(const real& dt) {
    solver->solve(dt, rigidbodies);
}

void PhysicsWorld::update_collision() {
    SweepAndPrune();
    return;
    for (auto& object1 : rigidbodies) {
        for (auto& object2 : rigidbodies) {
            if (object1 == object2) 
                break;
            if (!object1->getCollider().enable or !object2->getCollider().enable)
                continue;
            if (object1->isStatic and object2->isStatic)
                continue;
            auto state = object1->getCollider().testCollision(&object2->getCollider(), object1, object2);
            if (state.depth < 0) {
                resolve_velocity(state, object1, object2);
            }
        }
    }
}

struct Collision {
    CollisionState state;
    RigidBody2D *body1, *body2;
};

inline void resolve_velocity(Collision& collision) {
    resolve_velocity(collision.state, collision.body1, collision.body2);
}

void PhysicsWorld::SweepAndPrune() {
    std::sort(rigidbodies.begin(), rigidbodies.end(), [](RigidBody2D* body1, RigidBody2D* body2) {
                return body1->getCollider().getBounds().min.x < body2->getCollider().getBounds().min.x;
            });

    std::vector<ObjectContainer> possible_collision;
    possible_collision.reserve(100);
    possible_collision.emplace_back(ObjectContainer{});
    possible_collision.back().reserve(100);
    possible_collision.back().emplace_back(rigidbodies[0]);

    auto aabb = rigidbodies[0]->getCollider().getBounds();
    real right = aabb.max.x;

    for (u64 i = 1; i < rigidbodies.size(); ++i) {
        auto aabb = rigidbodies[i]->getCollider().getBounds();
        if (aabb.min.x < right) {
            possible_collision.back().emplace_back(rigidbodies[i]);
            right = aabb.max.x > right ? aabb.max.x : right;
            continue;
        }
        right = aabb.max.x;
        possible_collision.emplace_back(ObjectContainer{});
        possible_collision.back().reserve(100);
    }

#if 1
    std::vector<Collision> collisions;
    std::mutex mtx;
    std::for_each(std::execution::par_unseq, possible_collision.begin(), possible_collision.end(),
        [&collisions, &mtx](ObjectContainer& objects) {
        std::vector<Collision> result;
        result.reserve(10);
        for (auto& object1 : objects) {
            for (auto& object2 : objects) {
                if (object1 == object2)
                    break;
                if (!object1->getCollider().enable or !object2->getCollider().enable)
                    continue;
                if (object1->isStatic and object2->isStatic)
                    continue;
                if (!TestCollision2D::AABB(object1->getCollider().getBounds(), object2->getCollider().getBounds()))
                    continue;
                auto state = object1->getCollider().testCollision(&object2->getCollider(), object1, object2);
                if (state.depth < 0) {
                    result.push_back(Collision{state, object1, object2});
                }
            }
        }
        if (mtx.try_lock()) {
            collisions.insert(collisions.end(), result.begin(), result.end());
            mtx.unlock();
        }
    });
    std::for_each(std::execution::par_unseq, collisions.begin(), collisions.end(), 
        [](Collision& collision) {
            resolve_velocity(collision);
        }
    );

#else
    std::for_each(possible_collision.begin(), possible_collision.end(),
        [](ObjectContainer& objects) {
            for (auto& object1 : objects) {
                for (auto& object2 : objects) {
                    if (object1 == object2)
                        break;
                    if (!object1->getCollider().enable or !object2->getCollider().enable)
                        continue;
                    if (object1->isStatic and object2->isStatic)
                        continue;
                    if (!TestCollision2D::AABB(object1->getCollider().getBounds(), object2->getCollider().getBounds()))
                        continue;
                    auto state = object1->getCollider().testCollision(&object2->getCollider(), object1, object2);
                    if (state.depth < 0) {
                    resolve_velocity(state, object1, object2);
                    }
                }
            }
        }
    );
#endif

}

void PhysicsWorld::KDTreesSpacePartitioning() {

}

void PhysicsWorld::update_constraint(const real& dt) {
    for (auto& constraint : constraints) {
        constraint->update(dt);
    }
}

