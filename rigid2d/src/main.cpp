#include "PhysicsEmulator.h"
#include "ObjectBuilder.h"

class Line {
public:
    vec2 p[2];
    Line(vec2 p1, vec2 p2) {
        p[0] = p1;
        p[1] = p2;
    }

    bool collide(Circle* circle) {
        const vec2 direction = p[1] - p[0];
        real target_length = glm::dot(circle->m_position - p[0], direction) / glm::length(direction);
        target_length = glm::clamp(target_length, 0.0, glm::length(direction));
        const vec2 target = target_length * glm::normalize(direction) + p[0];
        return glm::length(circle->m_position - target) < circle->radius;
    }

    void resolve_collision(Circle* circle) {
        const vec2 direction = p[1] - p[0];
        const real target_length = glm::clamp(glm::dot(circle->m_position - p[0], direction) / glm::length(direction), 0.0, glm::length(direction));
        const vec2 target = target_length * glm::normalize(direction) + p[0];
        const vec2 parallel_velocity = glm::dot(circle->m_velocity, direction) * glm::normalize(direction) / glm::length(direction);

        const real bounce = 0.3;
        const vec2 friction = -circle->m_mass * 9.81 * 0.1 * parallel_velocity;

        circle->m_position += (circle->radius - glm::length(circle->m_position - target)) * glm::normalize(circle->m_position - target);
        circle->m_velocity = parallel_velocity - (circle->m_velocity - parallel_velocity) * bounce;
        circle->addForce(friction);
    }

};

class DemoSimulation : public Simulation {
public:
    DemoSimulation()
        : Simulation("Demo", 0.3f)
    {
        attri.node_color = glm::vec3(COLOR(0x858AA6));

        initialize = [this]() {
            world.initialize();
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

            buildCircle(glm::vec2(1, 0), 0.3)
                ->m_mass = 1;

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
        };
    }

    Line line = Line({5, -3}, {-5, -3});
    void update(const real& dt) {
        world.update(dt);

        auto& objects = world.getObjects<Circle>();
        for (auto& obj : objects) {
            auto circle = static_cast<Circle*>(obj);
            if (line.collide(circle)) {
                line.resolve_collision(circle);
            }
        }
    }

    void render(mfw::Renderer& renderer) {
        glm::mat4 proj = camera.getProjection();
        world.render(proj, renderer);

        renderer.renderLine(proj, line.p[0], line.p[1], glm::vec3(1), 0.01);
    }

};

Simulation* Simulation::Instance = new DemoSimulation();

mfw::Application* mfw::CreateApplication() {
    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->world_scale = 7 * Simulation::Get()->getWorldScale();
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;

    emulator->settings.sub_step = 1000;

    return emulator;
}

