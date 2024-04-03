#include "PhysicsEmulator.h"
#include <Clock.h>

#include "PointConstraint.h"
#include "DistanceConstraint.h"
#include "Circle.h"
#include "Simulation.h"
#include "Tracer.h"
#include "FixPoint.h"
#include "Roller.h"
#include "Rotator.h"

void wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
    static f64 bounce = 0.1;
    glm::vec2 a{}, s{};
    if (c->m_pos.y - c->r < -world.y) {
        s.y = -world.y - c->m_pos.y + c->r;
        a.y = (c->m_velocity.y * (-bounce  - 1)) / dt;
    }
    if (c->m_pos.x - c->r < -world.x) {
        s.x = -world.x - c->m_pos.x + c->r;
        a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
    }
    else if (c->m_pos.x + c->r > world.x) {
        s.x = world.x - c->m_pos.x - c->r;
        a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
    }
    c->m_pos += s;
    c->m_acceleration += a;
};

class DemoSimulation : public Simulation {
public:
    DemoSimulation()
        : Simulation("Demo", 0.2f)
    {
        f32 worldScale = getWorldScale();
        attri.node_color = glm::vec3(COLOR(0x858AA6));
        world = World(glm::vec2(30, 12) * worldScale);

        initialize = [this, worldScale]() {
            world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
            world.setObjectLayer<Circle>(RenderLayer::Level3);
            world.setObjectLayer<Tracer>(RenderLayer::Level4);
            world.setObjectLayer<Rotator>(RenderLayer::Level3);

            addString(this, {}, 2, 3 * worldScale);
            auto c1 = world.getObjects<Circle>()[0];
            auto c2 = world.getObjects<Circle>()[1];
            c1->m_pos = glm::vec2(0, 1.5) * worldScale;
            c2->m_pos = glm::vec2(0, -1.5) * worldScale;

            // 2-rotator
            auto c0 = world.addObject<Circle>(glm::vec2(0), attri.node_color, 0);
            addFixPointConstraint(this, {})->target = c0;
            auto rotator1 = world.addConstraint<Rotator>();
            rotator1->center = c0;
            rotator1->target = c1;
            rotator1->r = 1.5 * worldScale;
            rotator1->w = 1;
            rotator1 = world.addConstraint<Rotator>();
            rotator1->center = c0;
            rotator1->target = c2;
            rotator1->r = 1.5 * worldScale;
            rotator1->w = 1;

            c0 = world.addObject<Circle>(glm::vec2(0), attri.node_color, attri.node_size);
            world.addConstraint<DistanceConstraint>(c0, c1, 1 * worldScale, attri.line_width);
            rotator1 = world.addConstraint<Rotator>();
            rotator1->center = c1;
            rotator1->target = c0;
            rotator1->r = 1 * worldScale;
            rotator1->w = 1.5;

            auto tracer = world.addObject<Tracer>();
            tracer->target = c0;
            tracer->maxScale = 0.03 * worldScale;
            tracer->minScale = 0.01 * worldScale;
            tracer->maxSamples = 400;
            tracer->dr = 0.75;

            return;
            addDoublePendulum(this, 30, 3);

            for (i32 i = 0; i < 4; i++) {
                addFixPointConstraint(this, glm::vec2(-4, 4) * worldScale);
            }
            for (i32 i = 0; i < 4; i++) {
                addHorizontalPointConstraint(this, glm::vec2(4, 4) * worldScale);
            }
            // ::SetupRotateBox(this);
            addTriangle(this, glm::vec2(), 2 * worldScale);
            addBox(this, glm::vec2(), 2 * worldScale);
        };
    }

    void update(const f64& dt) {
        world.update(dt);

        // 2-rotator
        auto c1 = world.getObjects<Circle>()[0];
        auto c2 = world.getObjects<Circle>()[1];
        auto c0 = world.getObjects<Circle>()[2];
        glm::dvec2 s = (c0->m_pos - (c1->m_pos + c2->m_pos) * 0.5) * 0.5;
        c1->m_pos += s;
        c2->m_pos += s;
        c0->m_pos -= s;

        for (auto& obj : world.getObjects<Circle>()) {
            wall_collision(dt, static_cast<Circle*>(obj), world.size);
        }
    }

    void render(mfw::Renderer& renderer) {
        glm::mat4 proj = camera.getProjection();
        world.render(proj, renderer);

        f32 unitScale = getWorldScale();
        renderer.renderLine(proj, glm::vec2(world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec2(-world.size.x, -world.size.y) - 0.2f * unitScale, glm::vec4(1), 0.2 * unitScale);
    }

};

Simulation* Simulation::Instance = new DemoSimulation();

mfw::Application* mfw::CreateApplication() {
    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->world_scale = 7 * Simulation::Get()->getWorldScale();
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;

    emulator->settings.sub_step = 100;

    return emulator;
}

