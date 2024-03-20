#pragma once
#include "Scene.h"

class TestScene : public Scene {
public:
    f32 unitScale = 0.2;
    TestScene();

    virtual void update(const f64& dt) override;
    virtual void render(mfw::Renderer& renderer) override;
    virtual void reset() override;

private:
    void SetupDoublePendulum(f64 angle, f64 d);
    void SetupRotateBox();
    void InitializePointConstraint();
    void SetDefaultStickAttribute();

};
