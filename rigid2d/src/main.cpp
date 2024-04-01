#include "PhysicsEmulator.h"
#include "DemoSimulation.h"

#include "DistanceConstraint.h"
#include "Circle.h"
#include "Simulation.h"
#include "Tracer.h"

Simulation* Simulation::Instance = new DemoSimulation();

mfw::Application* mfw::CreateApplication() {
    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->world_scale = 7 * Simulation::Get()->getWorldScale();
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;

    emulator->settings.sub_step = 1000;

    return emulator;
}

