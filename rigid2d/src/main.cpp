#include "PhysicsEmulator.h"
#include "DemoSimulation.h"
#include "Circle.h"

mfw::Application* mfw::CreateApplication() {
#if 1
    Simulation* sim = new Simulation("Double Pendulum");
    sim->unitScale = 0.35f;
    sim->world = World(glm::vec2(30, 12) * sim->unitScale);
    sim->attri.node_color = glm::vec4(glm::vec4(COLOR(0x858AA6), 0));
    sim->attri.node_size = 0.24 * sim->unitScale;
    sim->attri.line_width = 0.08 * sim->unitScale;
    sim->attri.hardness = 1;

    sim->initialize = [sim]() {
        sim->addDoublePendulum(30, 4);
        sim->addTracer(sim->world, sim->world.getObjects<Circle>().back());
    };

    return new PhysicsEmulator(sim);
#else
    DemoSimulation* demo = new DemoSimulation();
    return new PhysicsEmulator(demo);
#endif
}

