#pragma once
#include "PointConstraint.h"
#include "World.h"
#include "Camera.h"

namespace mfw {
    class Renderer;
    class Window;
};
class Roller;
class FixPoint;
class Simulation {
public:
    struct Attribute {
        glm::vec4 node_color;
        f32 node_size;
        f32 line_width;
    };

    Simulation(const std::string& name, f32 unitScale, Attribute attri)
        : name(name), unitScale(unitScale), attri(attri)
    {
        attri.node_size *= unitScale;
        attri.line_width *= unitScale;
    }
    virtual ~Simulation() = default;
    virtual void update(const f64& dt);
    virtual void render(mfw::Renderer& renderer);

    std::function<void()> initialize;

    World world;
    Camera camera;

    std::string name;
    f32 unitScale = 0;
    Attribute attri;

    glm::dvec2 mouseToWorldCoord();

};

void addString(Simulation* sim, const glm::vec2& pos, u32 node, f32 length);
void addCircle(Simulation* sim, const glm::vec2& pos, i32 n, f32 r, i32 nstep = 1);
void addBox(Simulation* sim, const glm::vec2& pos, f32 l);
void addTriangle(Simulation* sim, const glm::vec2& pos, f32 l);
void addDoublePendulum(Simulation* sim, f64 angle, f64 d);
void SetupRotateBox(Simulation* sim);
PointConstraint* addHorizontalPointConstraint(Simulation* sim, const glm::dvec2& pos);
PointConstraint* addFixPointConstraint(Simulation* sim, const glm::dvec2& pos);

