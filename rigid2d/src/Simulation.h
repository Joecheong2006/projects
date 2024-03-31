#pragma once
#include "util.h"
#include "World.h"
#include "Camera.h"
#include <memory>

namespace mfw {
    class Renderer;
};
class PointConstraint;
class Simulation {
public:
    static inline std::shared_ptr<Simulation> Get() { return Instance; }
    template <typename T>
    static inline void Create(const T& sim) {
        Instance = std::make_shared<T>(sim);
    }

public:
    struct Attribute {
        glm::vec4 node_color = glm::vec4(0.9, 0.9, 0.9, 1);
        f32 node_size = 0.15f;
        f32 line_width = 0.12f;
    };

    Simulation(const std::string& name, f32 worldScale)
        : name(name), unitScale(worldScale)
    {
        attri.node_size *= unitScale;
        attri.line_width *= unitScale;
    }

    virtual ~Simulation() = default;
    virtual void update(const f64& dt);
    virtual void render(mfw::Renderer& renderer);
    inline f32 getWorldScale() const { return unitScale; }

    std::function<void()> initialize;

    World world;
    Camera camera;

    std::string name;
    Attribute attri;

    glm::dvec2 mouseToWorldCoord();

private:
    static std::shared_ptr<Simulation> Instance;
    f32 unitScale = 0;

};

void addString(Simulation* sim, const glm::vec2& pos, u32 node, f32 length);
void addCircle(Simulation* sim, const glm::vec2& pos, i32 n, f32 r, i32 nstep = 1);
void addBox(Simulation* sim, const glm::vec2& pos, f32 l);
void addTriangle(Simulation* sim, const glm::vec2& pos, f32 l);
void addDoublePendulum(Simulation* sim, f64 angle, f64 d);
void SetupRotateBox(Simulation* sim);
PointConstraint* addHorizontalPointConstraint(Simulation* sim, const glm::dvec2& pos);
PointConstraint* addFixPointConstraint(Simulation* sim, const glm::dvec2& pos);

