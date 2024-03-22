#pragma once
#include "Simluation.h"

class DemoSimluation : public Simluation {
public:
    DemoSimluation();

    virtual void update(const f64& dt) override;
    virtual void render(mfw::Renderer& renderer) override;
    virtual void reset() override;

};
