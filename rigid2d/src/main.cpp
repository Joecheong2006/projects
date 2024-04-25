#include "PhysicsEmulator.h"
#include "BuildObject.h"

class Line {
public:
    vec2 p[2];
    real restitution, friction_coefficient;
    Line(vec2 p1, vec2 p2, real restitution = 1, real friction = 0.1)
        : restitution(restitution), friction_coefficient(friction)
    {
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

    void resolve_velocity(Circle* circle) {
        const vec2 line_tangent = glm::normalize(p[1] - p[0]);
        const real tangent_length = glm::length(p[1] - p[0]);
        const real contactOffset = glm::clamp(glm::dot(circle->m_position - p[0], line_tangent), 0.0, tangent_length);
        const vec2 contact = contactOffset * line_tangent + p[0];

        const vec2 normal = glm::normalize(circle->m_position - contact);
        const vec2 tangent = glm::normalize(vec2(normal.y, -normal.x));
        const vec2 normal_velocity = glm::dot(circle->m_velocity, -normal) * -normal;
        const vec2 tangent_velocity = glm::dot(circle->m_velocity, tangent) * tangent;

        circle->m_position += (circle->radius - glm::length(circle->m_position - contact)) * normal;
        circle->m_velocity -= normal_velocity * (1 + restitution);

        if (tangent_velocity.x != 0 || tangent_velocity.y != 0) {
            const real n = -abs(glm::dot(Simulation::Get()->world.gravity, normal)) * circle->m_mass;
            circle->addForce(n * friction_coefficient * glm::clamp(tangent_velocity, vec2(-1), vec2(1)));
            const vec3 rvec = -vec3(normal, 0);
            const real rotate_direction = glm::normalize(glm::cross(rvec, vec3(-tangent_velocity, 0))).z;
            circle->m_angular_velocity = rotate_direction * glm::length(tangent_velocity) / circle->radius;
        }
    }

    void resolve_collision(Circle* circle) {
        resolve_velocity(circle);
    }

    void draw(const mat4& proj, mfw::Renderer& renderer) {
        renderer.renderLine(proj, p[0], p[1], glm::vec3(1), 0.01);
        renderer.renderCircle(proj, p[0], 0.01f, glm::vec4(1));
        renderer.renderCircle(proj, p[1], 0.01f, glm::vec4(1));
    }

};

class DemoSimulation : public Simulation {
public:
    DemoSimulation(): Simulation("Demo", 0.3f)
    {
        initialize = [&]() {
            world.initialize();
            world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
            world.setObjectLayer<Spring>(RenderLayer::Level2);
            world.setObjectLayer<Circle>(RenderLayer::Level3);
            world.setObjectLayer<Rotator>(RenderLayer::Level3);
            world.setObjectLayer<Tracer>(RenderLayer::Level4);

            auto o1 = BuildObject<Circle>({0, 0});
            auto o2 = BuildObject<Circle>({0, -2});
            auto o3 = BuildObject<Circle>({0, -4});
            o3->m_mass = 0.003; 
            o2->m_mass = 0.003; 
            BuildObject<FixPoint>(o1->m_position)
                ->target = o1;
            BuildObject<Spring>(o1, o2, 2, 50, 0.1, 8);
            BuildObject<Spring>(o2, o3, 2, 50, 0.1, 8);

            auto o2_tracer = BuildObject<Tracer>(o2, 0.1, 0.01, 0.7, 50);
            o2_tracer->m_color = glm::vec3(COLOR(0x7b7694));
            auto o3_tracer = BuildObject<Tracer>(o3, 0.1, 0.01, 0.7, 80);
            o3_tracer->m_color = glm::vec3(COLOR(0xcba987));

            o1 = BuildObject<Circle>({-1.5, 0});
            o2 = BuildObject<Circle>({-4, 0});
            BuildObject<FixPoint>(o1->m_position)
                ->target = o1;
            BuildObject<Roller>(o2->m_position)
                ->target = o2;
            BuildObject<Spring>(o1, o2, 2.5, 2, 0.1)->count = 8;

            auto circle = BuildObject<Circle>(glm::vec2(1, 0), 0.3);
            circle->m_mass = 1;

            circle = BuildObject<Circle>(glm::vec2(0, 0), 0.4);
            circle->m_mass = 0.5;
            auto circle1 = BuildObject<Circle>(glm::vec2(2, 0), 0.4);
            circle1->m_mass = 0.5;
            BuildObject<Spring>(circle, circle1, 2, 90, 2, 8);

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

    Line ground = Line({5, -3}, {-5, -3}, 0.2, 0.1);
    Line b1 = Line({1.4, -3}, {4, -2.2}, 0.4, 0.1);
    Line b2 = Line({-1.4, -3}, {-4, -2.2}, 0.4, 0.05);
    void update(const real& dt) {
        auto& objects = world.getObjects<Circle>();
        for (auto& obj : objects) {
            auto circle = static_cast<Circle*>(obj);
            if (ground.collide(circle)) {
                ground.resolve_collision(circle);
            }
            if (b1.collide(circle)) {
                b1.resolve_collision(circle);
            }
            if (b2.collide(circle)) {
                b2.resolve_collision(circle);
            }
        }
        world.update(dt);
    }

    void render(mfw::Renderer& renderer) {
        glm::mat4 proj = camera.getProjection();
        world.render(proj, renderer);

        ground.draw(proj, renderer);
        b1.draw(proj, renderer);
        b2.draw(proj, renderer);
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
    BuildObject<Circle>::default_d = 0.17;
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
    emulator->setSimulation(new DemoSimulation);

    emulator->world_scale = 6;
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.015 * emulator->world_scale;
    emulator->settings.sub_step = 500;
    
    return emulator;
    
}

