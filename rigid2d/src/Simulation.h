#pragma once
#include "World.h"
#include "Camera.h"

namespace mfw {
    class Renderer;
    class Window;
};
class PointConstraint;
struct Simulation {
    Simulation(const std::string& name): name(name) {}
    virtual ~Simulation() {}
    virtual void update(const f64& dt);
    virtual void render(mfw::Renderer& renderer);

    std::function<void()> initialize;

    World world;
    Camera camera;
    f32 unitScale = 0;
    std::string name;

    struct Attribute {
        glm::vec4 node_color;
        f32 node_size;
        f32 hardness;
        f32 line_width;
    } attri;

    glm::dvec2 mouseToWorldCoord();

    void addString(const glm::vec2& pos, u32 node, f32 length);
    void addCircle(const glm::vec2& pos, i32 n, f32 r, i32 nstep = 1);
    void addBox(const glm::vec2& pos, f32 l);
    void addTriangle(const glm::vec2& pos, f32 l);
    void addDoublePendulum(f64 angle, f64 d);
    void SetupRotateBox();

    PointConstraint* addHorizontalPointConstraint(const glm::dvec2& pos, f32 r);
    PointConstraint* addFixPointConstraint(const glm::dvec2& pos, f32 r);
    void addTracer(World& world, Object* target, i32 samples = 120);

};
