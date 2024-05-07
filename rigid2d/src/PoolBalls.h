#pragma once
#include "Simulation.h"
#include <mfw/Input.h>

static color pool_ball_colors[] = {
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

class PoolBalls : public Simulation {
public:
    PoolBalls(): Simulation(__func__, 0.2) {}
    Circle holes[6];

    virtual void OnStart() override {
        SetDefaultConfig();
        world.setSubStep(6);
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
    }

    real angle = glm::acos(-1), strength = 3;
    vec2 direction;
    virtual void OnUpdate(const real& dt) override {
        static bool down = false;
        direction = vec2(glm::cos(angle), glm::sin(angle));
        auto balls = world.findObjects<Circle>();

        angle += dt * mfw::Input::KeyPress(MF_KEY_X) * 2;
        angle -= dt * mfw::Input::KeyPress(MF_KEY_C) * 2;
        strength += dt * mfw::Input::KeyPress(MF_KEY_S) * 10;
        strength -= dt * mfw::Input::KeyPress(MF_KEY_D) * 10;

        static i32 count = 0;
        if (mfw::Input::KeyPress(MF_KEY_S) && count++ % 4 == 0) {
            auto circle = world.addRigidBody<Circle>(vec2(0, 1), 0.04, pool_ball_colors[count % 15]);
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

        for (auto& circle : balls) {
            if (circle == balls.back())
                continue;
            for (auto& hole : holes) {
                auto state = circle->RigidBody::collider->testCollision(&hole.collider, circle, &hole);
                if (state.depth < 0) {
                    world.destoryRigidBody(circle);
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

    void CreateBox(real width, real height, vec2 offset = vec2(0)) {
        auto& world = Simulation::Get()->world;
        world.addRigidBody<Cylinder>(vec2{width, -height} + offset, vec2{-width, -height} + offset);
        world.addRigidBody<Cylinder>(vec2{-width, -height} + offset, vec2{-width, height} + offset);
        world.addRigidBody<Cylinder>(vec2{-width, height} + offset, vec2{width, height} + offset);
        world.addRigidBody<Cylinder>(vec2{width, height} + offset, vec2{width, -height} + offset);
    }

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
                auto circle = world.addRigidBody<Circle>(vec2(-pos.y, pos.x) + vec2(-1.7, 0), radius, pool_ball_colors[color_id++]);
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

};
