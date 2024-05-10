#pragma once
#include "Simulation.h"

class DoublePendulum : public Simulation {
public:
    DoublePendulum(): Simulation(__func__, 0.1) {}
    virtual void OnStart() override {
        SetDefaultConfig();
        world.setSubStep(100);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level1);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);

        spring_double_pendulum(0.5, 10);
    }

    void spring_double_pendulum(const real len, const real angle) {
        const real r = angle * 3.14 / 180;
        vec2 direction = glm::normalize(vec2(cos(r), sin(r))) * len;

        auto o1 = world.addRigidBody<Circle>(vec2{0, 0});
        o1->getCollider().enable = false;
        auto fix = world.addConstraint<FixPoint>(o1->m_position);
        fix->drawEnable = false;
        fix->setTarget(o1);

        auto o2 = world.addRigidBody<Circle>(direction);
        auto o3 = world.addRigidBody<Circle>(direction * 2.0);

        o2->setMass(0.001);
        o3->setMass(0.001);

        world.addConstraint<Spring>(o1, o2, len, 50, 0.1, 8);
        world.addConstraint<Spring>(o2, o3, len, 50, 0.1, 8);

        auto o2_tracer = world.addConstraint<Tracer>(o2, 0.2_mu, 0.02_mu, 0.7, 50);
        auto o3_tracer = world.addConstraint<Tracer>(o3, 0.2_mu, 0.02_mu, 0.7, 80);

        o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
        o3_tracer->m_color = glm::vec3(COLOR(0xcba987));
    };

};

