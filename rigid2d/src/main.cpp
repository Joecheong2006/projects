#include "PhysicsEmulator.h"
#include "mp/BuildObject.h"
#include <mfw/Input.h>

void CreateBox(real width, real height, vec2 offset = vec2(0)) {
    auto& world = Simulation::Get()->world;
    world.addRigidBody<Cylinder>(vec2{width, -height} + offset, vec2{-width, -height} + offset);
    world.addRigidBody<Cylinder>(vec2{-width, -height} + offset, vec2{-width, height} + offset);
    world.addRigidBody<Cylinder>(vec2{-width, height} + offset, vec2{width, height} + offset);
    world.addRigidBody<Cylinder>(vec2{width, height} + offset, vec2{width, -height} + offset);
}

void cloth(vec2 size, vec2 count, real stiffness, real damping, vec2 offset = vec2(0)) {
    auto& world = Simulation::Get()->world;
    const vec2 persize = size / count;
    auto& objects = world.getObjects<Circle>();
    const i32 startIndex = objects.size();

    offset -= vec2(size.x * 0.5, 0);

    for (i32 i = 0; i < count.y; i++) {
        for (i32 j = 0; j < count.x; j++) {
            auto obj = world.addRigidBody<Circle>(vec2(j, -i) * persize + offset);
            obj->setMass(0.004);
            obj->radius = 0.12_mu;
            obj->m_damping = 0.6;
            obj->RigidBody::collider = nullptr;
        }
    }

    for (i32 i = 0; i < count.x; i++) {
        auto target = static_cast<Circle*>(objects[startIndex + i]);
        world.addConstraint<FixPoint>(target->m_position, target->radius * 1.5)->setTarget(target);
    }

    const real pre_len = Spring::default_w;
    const color pre_color = Spring::default_color;
    Spring::default_w = 0.03_mu;
    Spring::default_color = {COLOR(0xaaaaaa)};
    for (i32 i = 0; i < count.y; i++) {
        for (i32 j = 0; j < count.x - 1; j++) {
            world.addConstraint<Spring>(
                    objects[startIndex + j + i * count.x],
                    objects[startIndex + j + i * count.x + 1], persize.x, stiffness, damping)
                ->count = 1;
        }
    }
    for (i32 i = 0; i < count.y - 1; i++) {
        for (i32 j = 0; j < count.x; j++) {
            world.addConstraint<Spring>(
                    objects[startIndex + j + i * count.x],
                    objects[startIndex + j + (i + 1) * count.x], persize.y, stiffness, damping)
                ->count = 1;
        }
    }
    Spring::default_w = pre_len;
    Spring::default_color = pre_color;
}

void spring_double_pendulum() {
    const real len = 0.5;

    auto& world = Simulation::Get()->world;
    auto o1 = world.addRigidBody<Circle>(vec2{0, 0});
    o1->RigidBody::collider = nullptr;
    world.addConstraint<FixPoint>(o1->m_position)
        ->setTarget(o1);

    auto o2 = world.addRigidBody<Circle>(vec2{0, -len});
    o2->setMass(0.001);
    auto o3 = world.addRigidBody<Circle>(vec2{0, -len * 2});
    o3->setMass(0.001);
    world.addConstraint<Spring>(o1, o2, len, 50, 0.1, 8);
    world.addConstraint<Spring>(o2, o3, len, 50, 0.1, 8);

    auto o2_tracer = world.addConstraint<Tracer>(o2, 0.1_mu, 0.01_mu, 0.7, 50);
    o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
    auto o3_tracer = world.addConstraint<Tracer>(o3, 0.1_mu, 0.01_mu, 0.7, 80);
    o3_tracer->m_color = glm::vec3(COLOR(0xcba987));
};

void cradle(i32 count, real length, real space, vec2 offset = {0, 0}) {
    offset -= count * space * 0.5;
    auto& world = Simulation::Get()->world;
    real radius = space * 0.5;
    for (real i = 0; i < count; i++) {
        auto circle1 = world.addRigidBody<Circle>(vec2(space, 0) * i + offset, radius);
        circle1->setStatic();
        auto circle2 = world.addRigidBody<Circle>(vec2(space * i, -length) + offset, radius);
        circle2->setMass(0.1);
        circle2->m_restitution = 0.96;
        world.addConstraint<DistanceConstraint>(circle1, circle2, length, 0.14_mu)
            ->color = {COLOR(0xefefef)};
    }
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

void pool_balls() {
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

class MonoBehaviour {
    static void Destory(Object& object, real t = 0) {
    }
};

class Demo : public Simulation {
public:
    Demo(): Simulation("pool balls", 0.2) {}
    Circle holes[6];

    virtual void OnStart() override {
        Circle::default_color = {COLOR(0x858AA6)};
        Circle::default_radius = 0.2_mu;

        Cylinder::default_color = vec3(1.0);
        Cylinder::default_width = 0.04_mu;

        DistanceConstraint::default_color = {COLOR(0xefefef)};
        DistanceConstraint::default_w = 0.14_mu;

        Tracer::default_color = {COLOR(0xc73e3e)};
        Tracer::default_maxScale = 0.03_mu;
        Tracer::default_minScale = 0.01_mu;
        Tracer::default_dr = 0.75;
        Tracer::default_maxSamples = 450;

        FixPoint::default_color = {COLOR(0x486577)};
        FixPoint::default_d = Circle::default_radius * 1.6f;

        Roller::default_color = {COLOR(0x3c4467)};
        Roller::default_d = FixPoint::default_d;

        Spring::default_color = {COLOR(0xefefef)};
        Spring::default_w = 0.06_mu;
        Spring::default_stiffness = 14;
        Spring::default_damping = 0.3;

        world.setSubStep(5);
        world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
        world.setObjectLayer<Spring>(RenderLayer::Level1);
        world.setObjectLayer<Circle>(RenderLayer::Level3);
        world.setObjectLayer<Cylinder>(RenderLayer::Level8);

        i32 count = 0;
        for (i32 i = -1; i <= 1; i += 2) {
            for (i32 j = -1; j < 2; j++) {
                real r = 0.05;
                vec2 pos = vec2(3.569 * j, 1.778 * i) - vec2(r * j, r * i);
                holes[count++] = Circle(pos, r, vec3(1));
            }
        }

        pool_balls();
        // cradle(5, 1, 0.2, vec2(0, 1));
        // spring_double_pendulum();

        // cloth({4, 3}, {40, 20}, 20, 0.1, {0, 1.4});
        return;
        const real len = 0.5;
        auto c1 = world.addRigidBody<Circle>(vec2());
        c1->RigidBody::collider = nullptr;
        world.addConstraint<FixPoint>(c1->m_position)->setTarget(c1);
        auto c2 = world.addRigidBody<Circle>(vec2(len, 0));
        auto c3 = world.addRigidBody<Circle>(vec2(len * 2, 0));

        world.addConstraint<DistanceConstraint>(c1, c2, len);
        world.addConstraint<DistanceConstraint>(c2, c3, len);
        world.addConstraint<Rotator>(c1, c2, len, 3.1415 * 1);
        world.addConstraint<Rotator>(c2, c3, len, 3.1415 * 4);
        world.addConstraint<Tracer>(c3, 0.2_mu, 0.02_mu, 0.7, 50);
    }

    real angle = glm::acos(-1), strength = 3;
    vec2 direction;
    virtual void OnUpdate(const real& dt) override {
        return;
        static bool down = false;
        direction = vec2(glm::cos(angle), glm::sin(angle));
        auto& balls = world.getObjects<Circle>();

        angle += dt * mfw::Input::KeyPress(MF_KEY_X) * 2;
        angle -= dt * mfw::Input::KeyPress(MF_KEY_C) * 2;
        strength += dt * mfw::Input::KeyPress(MF_KEY_S) * 10;
        strength -= dt * mfw::Input::KeyPress(MF_KEY_D) * 10;

        static i32 count = 0;
        if (mfw::Input::KeyPress(MF_KEY_S) && count++ % 4 == 0) {
            auto circle = world.addRigidBody<Circle>(vec2(0, 1), 0.04, colors[count % 15]);
            circle->setMass(0.01);
            circle->addForce(vec2{10, 0});
            circle->m_restitution = 0.4;
        }

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

        return;
        for (auto& circle : balls) {
            if (circle == balls.back())
                continue;
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

