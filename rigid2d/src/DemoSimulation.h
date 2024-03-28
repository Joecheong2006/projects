#pragma once
#include "Simulation.h"

class DemoSimulation : public Simulation {
public:
    DemoSimulation();

    virtual void update(const f64& dt) override;
    virtual void render(mfw::Renderer& renderer) override;

};
