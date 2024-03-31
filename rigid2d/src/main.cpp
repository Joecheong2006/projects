#include "PhysicsEmulator.h"
#include "DemoSimulation.h"

#include "DistanceConstraint.h"
#include "Circle.h"
#include "Simulation.h"
#include "Tracer.h"

mfw::Application* mfw::CreateApplication() {
#if 0
    Simulation* sim = new Simulation("Double Pendulum",
                0.4f,
                {
                    glm::vec4(COLOR(0x858AA6), 1),
                    0.06f,
                    0.05f,
                }
            );
    sim->world = World(glm::vec2(30, 12) * sim->unitScale);

    sim->initialize = [sim]() {
        auto& world = sim->world;
        auto& unitScale = sim->unitScale;
        world.setObjectLayer<Tracer>(RenderLayer::Level4);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        ::addDoublePendulum(sim, 30, 3);

        auto tracer = world.addObject<Tracer>();
        tracer->target = world.getObjects<Circle>().back();
        tracer->maxScale = 0.12 * unitScale;
        tracer->minScale = 0.01 * unitScale;
        tracer->dr = 0.75;
        tracer->maxSamples = 200;
    };

#else
    DemoSimulation* sim = new DemoSimulation();
#endif

    PhysicsEmulator* emulator = new PhysicsEmulator(sim);
    emulator->world_scale = 8 * sim->unitScale;
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;

    emulator->settings.sub_step = 1000;

    return emulator;
}

