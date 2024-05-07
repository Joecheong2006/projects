#include "PhysicsEmulator.h"

#include "Cradle.h"
#include "Cloth.h"
#include "PoolBalls.h"

void spring_double_pendulum() {
    const real len = 0.5;

    auto& world = Simulation::Get()->world;
    auto o1 = world.addRigidBody<Circle>(vec2{0, 0});
    o1->RigidBody::collider = nullptr;
    world.addConstraint<FixPoint>(o1->m_position)
        ->setTarget(o1);

    auto o2 = world.addRigidBody<Circle>(vec2{0, -len});
    o2->setMass(0.001);
    auto o3 = world.addRigidBody<Circle>(vec2{0, -len * 2});
    o3->setMass(0.001);
    world.addConstraint<Spring>(o1, o2, len, 50, 0.1, 8);
    world.addConstraint<Spring>(o2, o3, len, 50, 0.1, 8);

    auto o2_tracer = world.addConstraint<Tracer>(o2, 0.1_mu, 0.01_mu, 0.7, 50);
    o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
    auto o3_tracer = world.addConstraint<Tracer>(o3, 0.1_mu, 0.01_mu, 0.7, 80);
    o3_tracer->m_color = glm::vec3(COLOR(0xcba987));
};

namespace Log {
    template <>
    struct Pattern<vec2> {
        static void Log(const vec2& value, const std::string& format) {
            (void)format;
            LOG_INFO("[{}, {}]", value.x, value.y);
        }
    };
}

class Demo : public Simulation {
public:
    Demo(): Simulation(__func__, 0.2) {}
    virtual void OnStart() override {
        SetDefaultConfig();
        world.setSubStep(30);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level1);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);

        const real len = 0.5;
        auto c1 = world.addRigidBody<Circle>(vec2());
        c1->RigidBody::collider = nullptr;
        world.addConstraint<FixPoint>(c1->m_position)->setTarget(c1);
        auto c2 = world.addRigidBody<Circle>(vec2(len, 0));
        auto c3 = world.addRigidBody<Circle>(vec2(len * 2, 0));

        world.addConstraint<DistanceConstraint>(c1, c2, len);
        world.addConstraint<DistanceConstraint>(c2, c3, len);
        world.addConstraint<Rotator>(c1, c2, len, 3.1415 * 1);
        world.addConstraint<Rotator>(c2, c3, len, 3.1415 * 4);
        world.addConstraint<Tracer>(c3, 0.2_mu, 0.02_mu, 0.7, 50);
    }

};

mfw::Application* mfw::CreateApplication() {
    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->setSimulation(new PoolBalls);
    emulator->world_scale = 13;
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.015 * emulator->world_scale;

    return emulator;
}

