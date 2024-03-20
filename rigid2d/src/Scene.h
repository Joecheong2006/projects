#pragma once
#include "World.h"
#include "Camera.h"

namespace mfw {
    class Renderer;
    class Window;
};
class PointConstraint;
struct Scene {
    Scene() {}
    virtual ~Scene() {}
    virtual void update(const f64& dt) = 0;
    virtual void render(mfw::Renderer& renderer) = 0;
    virtual void reset() = 0;
    World world;
    Camera camera;

    struct Attribute {
        glm::vec4 node_color;
        f32 node_size;
        f32 hardness;
        f32 line_width;
    } attri;

    glm::dvec2 mouseToWorldCoord(mfw::Window* main);

protected:
    void InitString(World& world, const glm::vec2& pos, u32 node, f32 length);
    void InitCircle(World& world, const glm::vec2& pos, i32 n, f32 r, i32 nstep = 1);
    void InitBox(World& mesh, const glm::vec2& pos, f32 l);
    void InitTriangle(World& mesh, const glm::vec2& pos, f32 l);

    PointConstraint* AddHorizontalPointConstraint(World& world, const glm::dvec2& pos, f32 r);
    PointConstraint* AddFixPointConstraint(World& world, const glm::dvec2& pos, f32 r);

};
