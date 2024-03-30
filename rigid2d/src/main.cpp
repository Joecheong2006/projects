#include "PhysicsEmulator.h"
#include "DemoSimulation.h"

mfw::Application* mfw::CreateApplication() {
#if 0
    Simulation* sim = new Simulation("Double Pendulum");
    sim->unitScale = 0.35f;
    sim->world = World(glm::vec2(30, 12) * sim->unitScale);
    sim->attri.node_color = glm::vec4(COLOR(0xe2e2e2), 0);
    sim->attri.node_size = 0.18 * sim->unitScale;
    sim->attri.line_width = 0.12 * sim->unitScale;
    sim->attri.hardness = 1;

    sim->initialize = [sim]() {
        sim->addDoublePendulum(30, 4);
        sim->addTracer(sim->world, sim->world.getObjects<Circle>().back());
    };

#else
    DemoSimulation* sim = new DemoSimulation();
#endif

    PhysicsEmulator* emulator = new PhysicsEmulator(sim);
    emulator->world_scale = 7 * sim->unitScale;
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;

    emulator->settings.sub_step = 100;

    return emulator;
}

