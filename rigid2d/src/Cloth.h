#pragma once
#include "Simulation.h"

class Cloth : public Simulation {
public:
    Cloth(): Simulation(__func__, 0.1) {}
    virtual void OnStart() override {
        SetDefaultConfig();
        world.setSubStep(6);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level1);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);

        cloth({12.0_mu, 6.0_mu}, {40, 20}, 20, 0.1, {0, 3.0_mu});
    }

    void cloth(vec2 size, vec2 count, real stiffness, real damping, vec2 offset = vec2(0)) {
        auto& world = Simulation::Get()->world;
        const vec2 persize = size / count;
        auto objects = world.findObjects<Circle>();
        const i32 startIndex = objects.size();

        offset -= vec2(size.x * 0.5, 0);

        for (i32 i = 0; i < count.y; i++) {
            for (i32 j = 0; j < count.x; j++) {
                auto obj = world.addRigidBody<Circle>(vec2(j, -i) * persize + offset);
                obj->setMass(0.004);
                obj->radius = persize.x * 0.2;
                obj->m_damping = 0.6;
                obj->RigidBody::collider = nullptr;
            }
        }

        objects = world.findObjects<Circle>();
        for (i32 i = 0; i < count.x; i++) {
            auto target = static_cast<Circle*>(objects[startIndex + i]);
            world.addConstraint<FixPoint>(target->m_position, target->radius * 1.5)->setTarget(target);
        }

        const real pre_len = Spring::default_w;
        const color pre_color = Spring::default_color;
        Spring::default_w = 0.015_mu;
        Spring::default_color = {COLOR(0xaaaaaa)};
        for (i32 i = 0; i < count.y; i++) {
            for (i32 j = 0; j < count.x - 1; j++) {
                world.addConstraint<Spring>(
                        objects[startIndex + j + i * count.x],
                        objects[startIndex + j + i * count.x + 1], persize.x, stiffness, damping)
                    ->count = 1;
            }
        }
        for (i32 i = 0; i < count.y - 1; i++) {
            for (i32 j = 0; j < count.x; j++) {
                world.addConstraint<Spring>(
                        objects[startIndex + j + i * count.x],
                        objects[startIndex + j + (i + 1) * count.x], persize.y, stiffness, damping)
                    ->count = 1;
            }
        }
        Spring::default_w = pre_len;
        Spring::default_color = pre_color;
    }

};
