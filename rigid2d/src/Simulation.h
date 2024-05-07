#pragma once
#include "mp/util.h"
#include "mp/World.h"
#include "mp/Camera.h"

namespace mfw {
    class Renderer;
};
class PointConstraint;
class Simulation {
public:
    static Simulation* Get() { return Instance; }

public:
    Simulation(const std::string& name, real worldScale);

    virtual ~Simulation() = default;
    virtual void OnStart() {}
    virtual void OnUpdate(const real& dt) { (void)dt; }
    virtual void OnRender(mfw::Renderer& renderer) { (void)renderer; }
    real getWorldUnit() { return unitScale; }
    void initialize();

    PhysicsWorld world;
    Camera camera;
    std::string name;

    vec2 mouseToWorldCoord();

private:
    friend class PhysicsEmulator;

    void update(const real& dt);
    void render(mfw::Renderer& renderer);

    static Simulation* Instance;
    real unitScale;

};

void addString(const vec2& pos, u32 node, real length);
void addCircle(const vec2& pos, i32 n, real r, i32 nstep = 1);
void addBox(const vec2& pos, real l);
void addTriangle(const vec2& pos, real l);
void addDoublePendulum(real angle, real d);
void SetupRotateBox();
PointConstraint* addHorizontalPointConstraint(const vec2& pos);
PointConstraint* addFixPointConstraint(const vec2& pos);

real operator""_mu(const long double value);
real operator""_du(const long double value);
real operator""_cu(const long double value);
real operator""_mmu(const long double value);

