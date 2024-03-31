#include "PhysicsEmulator.h"
#include "DemoSimulation.h"

#include "DistanceConstraint.h"
#include "Circle.h"
#include "Simulation.h"
#include "Tracer.h"

mfw::Application* mfw::CreateApplication() {

#if 0
    Simulation::Create<Simulation>({"Double Pendulum", 0.4f});
    auto sim = Simulation::Get();
    sim->attri.node_color = glm::vec4(COLOR(0x858AA6), 1);
    sim->world = World(glm::vec2(30, 12) * sim->getWorldScale());

    sim->initialize = [sim]() {
        auto& world = sim->world;
        const f32 worldScale = sim->getWorldScale();
        world.setObjectLayer<Tracer>(RenderLayer::Level4);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        ::addDoublePendulum(sim.get(), 30, 3);

        auto tracer = world.addObject<Tracer>();
        tracer->target = world.getObjects<Circle>().back();
        tracer->maxScale = 0.12 * worldScale;
        tracer->minScale = 0.01 * worldScale;
    };

#else
    Simulation::Create<DemoSimulation>({});
    auto sim = Simulation::Get();
    sim->world.scale = sim->getWorldScale();
    sim->initialize = [sim]() {
        auto& world = sim->world;
        const f32 worldScale = sim->getWorldScale();

        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Tracer>(RenderLayer::Level4);

        for (i32 i = 0; i < 4; i++) {
            ::addFixPointConstraint(sim.get(), glm::vec2(-4, 4) * worldScale);
        }
        for (i32 i = 0; i < 4; i++) {
            ::addHorizontalPointConstraint(sim.get(), glm::vec2(4, 4) * worldScale);
        }
        ::SetupRotateBox(sim.get());
        ::addTriangle(sim.get(), glm::vec2(), 2 * worldScale);
        ::addBox(sim.get(), glm::vec2(), 2 * worldScale);
    };
#endif

    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->world_scale = 7 * Simulation::Get()->getWorldScale();
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;

    emulator->settings.sub_step = 1000;

    return emulator;
}

