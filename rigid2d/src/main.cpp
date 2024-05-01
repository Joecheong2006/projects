#include "PhysicsEmulator.h"
#include "BuildObject.h"

#define SCOPE(x) do x while (0)

void CreateCradle(float count, float space, float length,
        std::function<RigidBody*()> createCircle, vec2 offset = vec2(0)) {
    auto sim = Simulation::Get();
    const real worldScale = sim->getWorldScale();
    for (int i = 0; i < count; i++) {
        auto circle1 = createCircle();
        circle1->m_position = (vec2(i * space, 0) + offset) * worldScale;
        auto circle2 = createCircle();
        circle2->m_position = (vec2(i * space, -length) + offset) * worldScale;
        auto fix = BuildObject<FixPoint>(circle1->m_position);
        fix->target = circle1;
        fix->drawEnable = false;
        BuildObject<DistanceConstraint>(circle1, circle2, length);
    }
}

void CreateBox(real width, real height, vec2 offset = vec2(0)) {
    BuildObject<Line>(vec2{width, -height} + offset, vec2{-width, -height} + offset);
    BuildObject<Line>(vec2{-width, -height} + offset, vec2{-width, height} + offset);
    BuildObject<Line>(vec2{-width, height} + offset, vec2{width, height} + offset);
    BuildObject<Line>(vec2{width, height} + offset, vec2{width, -height} + offset);
}

static color colors[] = {
    color(COLOR(0xf4cd5d)),
    color(COLOR(0xfacc43)),
    color(COLOR(0x2459b1)),
    color(COLOR(0x2154b1)),
    color(COLOR(0x2f3135)),
    color(COLOR(0xf34a37)),
    color(COLOR(0xf84c46)),
    color(COLOR(0xc43030)),
    color(COLOR(0x36ad2f)),
    color(COLOR(0x564a96)),
    color(COLOR(0xfc8e1c)),
    color(COLOR(0xfd852a)),
    color(COLOR(0xc23032)),
    color(COLOR(0x2fa235)),
    color(COLOR(0x554896))
};

class DemoSimulation : public Simulation {
public:
    DemoSimulation(): Simulation("Demo", 0.3f)
    {
        Simulation::initialize = [] {
            Simulation* sim = Get();
            auto& world = sim->world;
            world.initialize();
            world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
            world.setObjectLayer<Spring>(RenderLayer::Level2);
            world.setObjectLayer<Circle>(RenderLayer::Level3);
            world.setObjectLayer<Rotator>(RenderLayer::Level3);
            world.setObjectLayer<Tracer>(RenderLayer::Level4);
            world.setObjectLayer<Line>(RenderLayer::Level8);

            // pool balls
            SCOPE ({
                world.gravity = {};
                const real worldScale = sim->getWorldScale();
                BuildObject<Circle>::default_restitution = 0.9;
                BuildObject<Circle>::default_d = 0.061 / worldScale;

                const real radius = BuildObject<Circle>::default_d;
                const real height = glm::sqrt(3 * radius * radius);
                const real damping = 0.3;
                const real mass = 0.210;
                const vec2 table_size = vec2(3.569 / worldScale, 1.778 / worldScale);

                i32 color_id = 0;
                for (i32 i = 0; i < 5; i++) {
                    for (i32 j = 0; j < i + 1; j++) {
                        vec2 pos = vec2(-j * 2 * radius + i * radius, i * height);
                        auto circle = BuildObject<Circle>(pos + vec2(0, 2));
                        circle->m_color = colors[color_id++];
                        circle->m_damping = damping;
                        circle->setMass(mass);
                    }
                }
                auto circle = BuildObject<Circle>({0, -4});
                circle->m_color = color(0.9);
                circle->m_damping = damping;
                circle->setMass(mass);
                CreateBox(table_size.x, table_size.y);
            });
            return;

            // cradle
            SCOPE ({
                Circle sample{};
                sample.m_restitution = 1;
                sample.radius = 0.1;
                sample.m_mass = 0.2;
                //CreateCradle(5, 0.65, 4, sample);
            });

            // string double pendulum
            SCOPE ({
                break;
                auto o1 = BuildObject<Circle>({0, 0});
                auto o2 = BuildObject<Circle>({0, -2});
                o2->setMass(0.003);
                auto o3 = BuildObject<Circle>({0, -4});
                o3->setMass(0.003);
                BuildObject<FixPoint>(o1->m_position)
                    ->target = o1;
                BuildObject<Spring>(o1, o2, 2, 50, 0.1, 8);
                BuildObject<Spring>(o2, o3, 2, 50, 0.1, 8);

                auto o2_tracer = BuildObject<Tracer>(o2, 0.1, 0.01, 0.7, 50);
                o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
                auto o3_tracer = BuildObject<Tracer>(o3, 0.1, 0.01, 0.7, 80);
                o3_tracer->m_color = glm::vec3(COLOR(0xcba987));
            });

            // testing
            SCOPE ({
                auto circle1 = BuildObject<Circle>(glm::vec2(2, 0), 0.4);
                circle1->setMass(0.5);
                auto circle2 = BuildObject<Circle>(glm::vec2(4, 0), 0.4);
                circle2->setMass(0.5);
                BuildObject<Spring>(circle1, circle2, 2, 90, 2, 8);
                BuildObject<Line>({15, -10}, {-13, -10});
                BuildObject<Line>({5, -10}, {13, -8});
                BuildObject<Line>({-5, -10}, {-20, -7});
            });

            return;
            BuildObject<Roller>({1, 0});

            BuildObject<FixPoint>({2, 0});
            BuildObject<FixPoint>({-2, 0});

            auto c1 = BuildObject<Circle>(glm::vec2(1.5));
            auto c0 = BuildObject<Circle>(glm::vec2(0), 0);
            BuildObject<DistanceConstraint>(c0, c1, 1.5);
            BuildObject<FixPoint>({})->target = c0;
            BuildObject<Rotator>(c0, c1, 1.5, 1);

            {
                auto c0 = BuildObject<Tracer>(BuildObject<Circle>(glm::vec2(0, 0.5)))->target;
                BuildObject<DistanceConstraint>(c0, c1, 1);
                BuildObject<Rotator>(c1, c0, 1, 1.5);
            }
        };
    }
};

namespace Log {
    template <>
    struct Pattern<vec2> {
        static void Log(const vec2& value, const std::string& format) {
            (void)format;
            LOG_INFO("[{}, {}]", value.x, value.y);
        }
    };
}

mfw::Application* mfw::CreateApplication() {
    BuildObject<Circle>::default_color = {COLOR(0x858AA6)};
    BuildObject<Circle>::default_restitution = 0.3;
    BuildObject<Circle>::default_d = 0.2;

    BuildObject<Line>::default_color = vec3(1);
    BuildObject<Line>::default_width = 0.04;

    BuildObject<DistanceConstraint>::default_color = {COLOR(0xefefef)};
    BuildObject<DistanceConstraint>::default_w = 0.14;

    BuildObject<Tracer>::default_color = {COLOR(0xc73e3e)};
    BuildObject<Tracer>::default_maxScale = 0.03;
    BuildObject<Tracer>::default_minScale = 0.01;
    BuildObject<Tracer>::default_dr = 0.75;
    BuildObject<Tracer>::default_maxSamples = 450;

    BuildObject<FixPoint>::default_color = {COLOR(0x486577)};
    BuildObject<FixPoint>::default_d = BuildObject<Circle>::default_d * 1.6f;

    BuildObject<Roller>::default_color = {COLOR(0x3c4467)};
    BuildObject<Roller>::default_d = BuildObject<FixPoint>::default_d;

    BuildObject<Spring>::default_color = {COLOR(0xefefef)};
    BuildObject<Spring>::default_w = 0.06;
    BuildObject<Spring>::default_stiffness = 14;
    BuildObject<Spring>::default_damping = 0.3;

    PhysicsEmulator* emulator = new PhysicsEmulator();


    auto cradle = [] {
        CreateCradle(5, 0.6, 6, [=]() {
                    auto circle = BuildObject<Circle>({}, 0.3, 1);
                    circle->setMass(0.2);
                    return circle;
                });
    };

    auto pool_balls = [] {
        auto sim = Simulation::Get();
        auto& world = sim->world;
        world.gravity = {};

        const real worldScale = sim->getWorldScale();
        BuildObject<Circle>::default_restitution = 0.9;
        BuildObject<Circle>::default_d = 0.061 / worldScale;

        const real radius = BuildObject<Circle>::default_d;
        const real height = glm::sqrt(3 * radius * radius);
        const real damping = 0.2;
        const real mass = 0.210;
        const vec2 table_size = vec2(3.569 / worldScale, 1.778 / worldScale);

        CreateBox(table_size.x, table_size.y);
        i32 color_id = 0;
        for (i32 i = 0; i < 5; i++) {
            for (i32 j = 0; j < i + 1; j++) {
                vec2 pos = vec2(-j * 2 * radius + i * radius, i * height);
                auto circle = BuildObject<Circle>(pos + vec2(0, 2));
                circle->m_color = colors[color_id++];
                circle->m_damping = damping;
                circle->setMass(mass);
            }
        }
        auto circle = BuildObject<Circle>({0, -4});
        circle->m_color = color(0.9);
        circle->m_damping = damping;
        circle->setMass(mass);
    };

    Simulation* sim = new Simulation("test", 0.2);
    sim->initialize = [=] {
        auto sim = Simulation::Get();
        auto& world = sim->world;
        world.initialize();
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level2);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Line>(RenderLayer::Level8);
        pool_balls();
        // cradle();
    };

    emulator->setSimulation(sim);
    emulator->world_scale = 13;
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.015 * emulator->world_scale;
    emulator->settings.sub_step = 500;
    
    return emulator;
    
}

