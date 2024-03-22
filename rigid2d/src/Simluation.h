#pragma once
#include "World.h"
#include "Camera.h"

namespace mfw {
    class Renderer;
    class Window;
};
class PointConstraint;
struct Simluation {
    static Simluation* Instance;

    Simluation(const std::string& name): name(name) {}
    virtual ~Simluation() {}
    virtual void update(const f64& dt) = 0;
    virtual void render(mfw::Renderer& renderer) = 0;
    virtual void reset() = 0;

    World world;
    Camera camera;
    f32 unitScale = 0.35;
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

    PointConstraint* addHorizontalPointConstraint(World& world, const glm::dvec2& pos, f32 r);
    PointConstraint* addFixPointConstraint(World& world, const glm::dvec2& pos, f32 r);

};
