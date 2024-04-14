#pragma once
#include "util.h"
#include "World.h"
#include "Camera.h"

namespace mfw {
    class Renderer;
};
class PointConstraint;
class Simulation {
public:
    static inline Simulation* Get() { return Instance; }
    template <typename T>
    static inline void Create(const T& sim) {
        Instance = new T(sim);
    }

public:
    Simulation(const std::string& name, real worldScale);

    virtual ~Simulation() = default;
    virtual void update(const real& dt);
    virtual void render(mfw::Renderer& renderer);
    inline real getWorldScale() const { return unitScale; }

    std::function<void()> initialize;

    World world;
    Camera camera;
    std::string name;

    vec2 mouseToWorldCoord();

private:
    static Simulation* Instance;
    real unitScale = 0;

};

void addString(const vec2& pos, u32 node, real length);
void addCircle(const vec2& pos, i32 n, real r, i32 nstep = 1);
void addBox(const vec2& pos, real l);
void addTriangle(const vec2& pos, real l);
void addDoublePendulum(real angle, real d);
void SetupRotateBox();
PointConstraint* addHorizontalPointConstraint(const vec2& pos);
PointConstraint* addFixPointConstraint(const vec2& pos);

