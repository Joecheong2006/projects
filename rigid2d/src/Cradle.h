#pragma once
#include "Simulation.h"

class Cradle : public Simulation {
public:
    Cradle(): Simulation(__func__, 0.03) {}
    virtual void OnStart() override {
        SetDefaultConfig();
        world.setSubStep(100);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level1);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);

        cradle(5, 7.0_mu, 0.015, {0, 0.1});
    }

    void cradle(i32 count, real length, real space, vec2 offset = {0, 0}) {
        offset -= count * space * 0.5;
        const real radius = space * 0.5;
        DistanceConstraint::default_w = radius * 0.3;
        for (real i = 0; i < count; i++) {
            auto circle1 = world.addRigidBody<Circle>(vec2(space, 0) * i + offset, radius);
            circle1->setStatic();
            auto circle2 = world.addRigidBody<Circle>(vec2(space * i, -length) + offset, radius);
            circle2->setMass(0.02);
            circle2->m_restitution = 1;
            world.addConstraint<DistanceConstraint>(circle1, circle2, length)
                ->color = {COLOR(0xefefef)};
        }
    };

};

