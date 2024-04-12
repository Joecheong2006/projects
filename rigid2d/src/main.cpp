#include "PhysicsEmulator.h"
#include "ObjectBuilder.h"

void wall_collision(f64 dt, Circle* c, const glm::vec2& world) {
    static f64 bounce = 0.1;
    glm::vec2 a{}, s{};
    if (c->m_position.y - c->radius < -world.y) {
        s.y = -world.y - c->m_position.y + c->radius;
        a.y = (c->m_velocity.y * (-bounce  - 1)) / dt;
    }
    if (c->m_position.x - c->radius < -world.x) {
        s.x = -world.x - c->m_position.x + c->radius;
        a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
    }
    else if (c->m_position.x + c->radius > world.x) {
        s.x = world.x - c->m_position.x - c->radius;
        a.x = (c->m_velocity.x * (-bounce - 1)) / dt;
    }
    c->m_position += s;
    c->m_acceleration += a;
};

class DemoSimulation : public Simulation {
public:
    DemoSimulation()
        : Simulation("Demo", 0.3f)
    {
        attri.node_color = glm::vec3(COLOR(0x858AA6));

        initialize = [this]() {
            world = World(glm::vec2(30, 12));
            world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
            world.setObjectLayer<Circle>(RenderLayer::Level3);
            world.setObjectLayer<Tracer>(RenderLayer::Level4);
            world.setObjectLayer<Rotator>(RenderLayer::Level3);

            auto buildLink = ObjectBuilder<DistanceConstraint>{glm::vec3(COLOR(0xefefef)), 0.14};
            auto buildCircle = ObjectBuilder<Circle>{attri.node_color, 0.17};
            auto buildTracer = ObjectBuilder<Tracer>{glm::vec3(COLOR(0xc73e3e)), 0.03, 0.01, 0.75, 450};
            auto buildRotator = ObjectBuilder<Rotator>{};
            auto buildFixPoint = ObjectBuilder<FixPoint>{glm::vec3(COLOR(0x486577)), buildCircle.default_d * 1.6f };
            auto buildRoller = ObjectBuilder<Roller>{glm::vec3(COLOR(0x3c4467)), buildCircle.default_d * 1.6f };
            auto buildSpring = ObjectBuilder<Spring>{glm::vec3(COLOR(0xefefef)), 0.06, 2, 0.1};

            auto o1 = buildCircle({0, 0});
            auto o2 = buildCircle({0, -2});
            auto o3 = buildCircle({0, -4});
            o3->m_mass = 0.003; 
            o2->m_mass = 0.003; 
            buildFixPoint(o1->m_position)
                ->target = o1;
            buildSpring(o1, o2, 2, 100, 0.1);
            buildSpring(o2, o3, 2, 100, 0.1);

            auto o2_tracer = buildTracer(o2);
            o2_tracer->maxSamples = 50;
            o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
            auto o3_tracer = buildTracer(o3);
            o3_tracer->maxSamples = 80;
            o3_tracer->m_color = glm::vec3(COLOR(0xcba987));

            o1 = buildCircle({-1.5, 0});
            o2 = buildCircle({-4, 0});
            buildFixPoint(o1->m_position)
                ->target = o1;
            buildRoller(o2->m_position)
                ->target = o2;
            buildSpring(o1, o2, 2.5);

            return;
            buildRoller({1, 0});

            buildFixPoint({2, 0});
            buildFixPoint({-2, 0});

            auto c1 = buildCircle(glm::vec2(1.5));
            auto c0 = buildCircle(glm::vec2(0), 0);
            buildLink(c0, c1, 1.5);
            buildFixPoint({})->target = c0;
            buildRotator(c0, c1, 1.5, 1);

            c0 = buildCircle(glm::vec2(0, 0.5));
            buildLink(c0, c1, 1);
            buildRotator(c1, c0, 1, 1.5);

            buildTracer(c0);

            buildCircle(glm::vec2(1, 0), 0.3)
                ->m_mass = 1;
        };
    }

    void update(const f64& dt) {
        world.update(dt);

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

    emulator->settings.sub_step = 6000;

    return emulator;
}

