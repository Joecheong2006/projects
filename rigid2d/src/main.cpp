#include "PhysicsEmulator.h"
#include "DemoSimulation.h"
#include "Circle.h"

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
    return new PhysicsEmulator(sim);
}

