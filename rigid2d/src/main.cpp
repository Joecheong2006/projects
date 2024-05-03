#include "PhysicsEmulator.h"
#include "BuildObject.h"
#include <mfw/Input.h>

void CreateCradle(float count, float space, float length,
        std::function<RigidBody*()> createCircle, vec2 offset = vec2(0)) {
    auto sim = Simulation::Get();
    const real worldScale = sim->getWorldUnit();
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
    auto& world = Simulation::Get()->world;
    world.addRigidBody<Cylinder>(vec2{width, -height} + offset, vec2{-width, -height} + offset);
    world.addRigidBody<Cylinder>(vec2{-width, -height} + offset, vec2{-width, height} + offset);
    world.addRigidBody<Cylinder>(vec2{-width, height} + offset, vec2{width, height} + offset);
    world.addRigidBody<Cylinder>(vec2{width, height} + offset, vec2{width, -height} + offset);
}


auto spring_double_pendulum = [] {
    auto& world = Simulation::Get()->world;
    auto o1 = world.addRigidBody<Circle>(vec2{0, 0});
    auto o2 = world.addRigidBody<Circle>(vec2{0, -2});
    o2->setMass(0.003);
    auto o3 = world.addRigidBody<Circle>(vec2{0, -4});
    o3->setMass(0.003);
    BuildObject<FixPoint>(o1->m_position)
        ->target = o1;
    BuildObject<Spring>(o1, o2, 2, 50, 0.1, 8);
    BuildObject<Spring>(o2, o3, 2, 50, 0.1, 8);

    auto o2_tracer = BuildObject<Tracer>(o2, 0.1, 0.01, 0.7, 50);
    o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
    auto o3_tracer = BuildObject<Tracer>(o3, 0.1, 0.01, 0.7, 80);
    o3_tracer->m_color = glm::vec3(COLOR(0xcba987));
};

auto cradle = [] {
    CreateCradle(5, 0.6, 6, [=]() {
                auto& world = Simulation::Get()->world;
                auto circle = world.addRigidBody<Circle>(vec2(), 0.3);
                circle->setMass(0.2);
                return circle;
            });
};

color colors[] = {
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

auto pool_balls = [] {
    auto& world = Simulation::Get()->world;
    world.gravity = {};

    const real restitution = 0.93;
    const real radius = 0.061;
    const real height = glm::sqrt(3 * radius * radius);
    const real damping = 0.3;
    const real mass = 0.21;
    const vec2 table_size = vec2(3.569, 1.778);

    i32 color_id = 0;
    for (i32 i = 0; i < 5; i++) {
        for (i32 j = 0; j < i + 1; j++) {
            vec2 pos = vec2(-j * 2 * radius + i * radius, i * height);
            auto circle = world.addRigidBody<Circle>(vec2(-pos.y, pos.x) + vec2(-1.7, 0), radius, colors[color_id++]);
            circle->m_damping = damping;
            circle->m_restitution = restitution;
            circle->setMass(mass);
        }
    }
    auto circle = world.addRigidBody<Circle>(vec2(1.4, 0), radius, color(0.9));
    circle->m_damping = damping;
    circle->m_restitution = restitution;
    circle->setMass(mass);
    CreateBox(table_size.x, table_size.y);
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

class Demo : public Simulation {
public:
    Demo(): Simulation("pool balls", 0.2) {}
    Circle holes[6];

    virtual void OnStart() override {
        Circle::default_color = {COLOR(0x858AA6)};
        Circle::default_radius = 0.2_mu;

        Cylinder::default_color = vec3(1.0);
        Cylinder::default_width = 0.04_mu;

        BuildObject<DistanceConstraint>::default_color = {COLOR(0xefefef)};
        BuildObject<DistanceConstraint>::default_w = 0.14;

        BuildObject<Tracer>::default_color = {COLOR(0xc73e3e)};
        BuildObject<Tracer>::default_maxScale = 0.03;
        BuildObject<Tracer>::default_minScale = 0.01;
        BuildObject<Tracer>::default_dr = 0.75;
        BuildObject<Tracer>::default_maxSamples = 450;

        BuildObject<FixPoint>::default_color = {COLOR(0x486577)};
        BuildObject<FixPoint>::default_d = Circle::default_radius * 1.6f;

        BuildObject<Roller>::default_color = {COLOR(0x3c4467)};
        BuildObject<Roller>::default_d = BuildObject<FixPoint>::default_d;

        BuildObject<Spring>::default_color = {COLOR(0xefefef)};
        BuildObject<Spring>::default_w = 0.06;
        BuildObject<Spring>::default_stiffness = 14;
        BuildObject<Spring>::default_damping = 0.3;

        world.setSubStep(5);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level2);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);
        i32 count = 0;
        for (i32 i = -1; i <= 1; i += 2) {
            for (i32 j = -1; j < 2; j++) {
                real r = 0.05;
                vec2 pos = vec2(3.569 * j, 1.778 * i) - vec2(r * j, r * i);
                holes[count] = Circle(pos, r, vec3(1));
                holes[count].setMass(1);
                count++;
            }
        }

        for (i32 i = 0; i < 2; i++) {
            for (i32 j = 0; j < 1; j++) {
                auto circle = world.addRigidBody<Circle>(vec2(i, j) * 0.1 - vec2(0.7), 0.04, colors[(i + j) % 15]);
                circle->m_restitution = 0.8;
                circle->setMass(0.02);
            }
        }

        pool_balls();

        // cradle();
    }

    real angle = glm::asin(-1), strength = 1;
    vec2 direction;
    virtual void OnUpdate(const real& dt) override {
        return;
        static bool down = false;
        direction = vec2(glm::cos(angle), glm::sin(angle));
        auto balls = world.getObjects<Circle>();

        angle += dt * mfw::Input::KeyPress(MF_KEY_X) * 2;
        angle -= dt * mfw::Input::KeyPress(MF_KEY_C) * 2;
        strength += dt * mfw::Input::KeyPress(MF_KEY_S) * 10;
        strength -= dt * mfw::Input::KeyPress(MF_KEY_D) * 10;

        if (mfw::Input::KeyPress(MF_KEY_Z)) {
            if (down)
                return;
            auto circle = balls.back();
            circle->addForce(direction * strength / dt);
            down = true;
        }
        else {
            down = false;
        }

        for (auto& circle : balls) {
            if (glm::length(circle->m_velocity) > 0) {
                real N = circle->m_mass * 9.81;
                circle->addForce(glm::normalize(circle->m_velocity) * -0.0001 * N / dt);
            }
        }

        for (i32 i = 0; i < (i32)balls.size(); i++) {
            auto& circle = balls[i];
            for (auto& hole : holes) {
                auto state = circle->collider->testCollision(&hole.collider, circle, &hole);
                if (state.depth < 0) {
                    world.destoryRigidBody<Circle>(circle);
                    break;
                }
            }
        }
    }

    virtual void OnRender(mfw::Renderer& renderer) override {
        auto proj = camera.getProjection();
        for (auto& hole : holes) {
            hole.draw(proj, renderer);
        }
        // auto circle = static_cast<Circle*>(world.getObjects<Circle>().back());
        // real len = circle->radius * strength;
        // renderer.renderLine(proj, circle->m_position, circle->m_position + direction * len, color(1, 0, 0), 0.01);
    }

};

mfw::Application* mfw::CreateApplication() {
    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->setSimulation(new Demo());
    emulator->world_scale = 13;
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.015 * emulator->world_scale;

    return emulator;
}

