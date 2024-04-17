#include "PhysicsEmulator.h"
#include "ObjectBuilder.h"

class Line {
public:
    vec2 p[2];
    real restitution;
    Line(vec2 p1, vec2 p2, real restitution = 1)
        : restitution(restitution)
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
        const vec2 normal = glm::normalize(vec2(p[0].y - p[1].y, p[1].x - p[0].x));
        const vec2 normal_velocity = glm::dot(circle->m_velocity, normal) * normal;
        const vec2 tangent_velocity = circle->m_velocity - normal_velocity;
        const vec2 friction = -circle->m_mass * 9.81 * 0.1 * (circle->m_velocity - normal_velocity);
        circle->addForce(friction);
        
        const vec2 tangent = glm::normalize(p[1] - p[0]);
        const real contactOffset = glm::dot(circle->m_position - p[0], tangent);
        const vec2 contact = contactOffset * tangent + p[0];
        circle->m_position += (circle->radius - glm::length(circle->m_position - contact)) * glm::normalize(circle->m_position - contact);
        circle->m_velocity -= normal_velocity * (1 + restitution);

        if (friction.x != 0 || friction.y != 0) {
            const vec3 rvec = vec3(contact - circle->m_position, 0);
            const real rotate_direction = glm::normalize(glm::cross(rvec, vec3(friction, 0))).z;
            circle->m_angular_velocity = rotate_direction * glm::length(tangent_velocity) / circle->radius;
        }

    }

    void resolve_collision(Circle* circle) {
        resolve_velocity(circle);
    }

};


template <typename T>
class BuildObject {};

template <>
class BuildObject<Circle> {
public:
    BuildObject(const vec2& position, const real& d, const color& color) {
        LOG_INFO("hi");
        object = 69;
    }

    operator real() const {
        return object;
    }

private:
    real object;

};

class DemoSimulation : public Simulation {
public:
    DemoSimulation()
        : Simulation("Demo", 0.3f)
    {
        initialize = [&]() {
            auto buildCircle = ObjectBuilder<Circle>();
            auto buildLink = ObjectBuilder<DistanceConstraint>();
            auto buildTracer = ObjectBuilder<Tracer>();
            auto buildRotator = ObjectBuilder<Rotator>();
            auto buildFixPoint = ObjectBuilder<FixPoint>();
            auto buildRoller = ObjectBuilder<Roller>();
            auto buildSpring = ObjectBuilder<Spring>();

            buildCircle.default_color = {COLOR(0x858AA6)};
            buildCircle.default_d = 0.17;
            buildLink.default_color = {COLOR(0xefefef)};
            buildLink.default_w = 0.14;

            buildTracer.default_color = {COLOR(0xc73e3e)};
            buildTracer.default_maxScale = 0.03;
            buildTracer.default_minScale = 0.01;
            buildTracer.default_dr = 0.75;
            buildTracer.default_maxSamples = 450;

            buildFixPoint.default_color = {COLOR(0x486577)};
            buildFixPoint.default_d = buildCircle.default_d * 1.6f;

            buildRoller.default_color = {COLOR(0x3c4467)};
            buildRoller.default_d = buildFixPoint.default_d;

            buildSpring.default_color = {COLOR(0xefefef)};
            buildSpring.default_w = 0.06;
            buildSpring.default_stiffness = 2;
            buildSpring.default_damping = 0.1;

            world.initialize();
            world.setObjectLayer<DistanceConstraint>(RenderLayer::Level2);
            world.setObjectLayer<Spring>(RenderLayer::Level2);
            world.setObjectLayer<Circle>(RenderLayer::Level3);
            world.setObjectLayer<Rotator>(RenderLayer::Level3);
            world.setObjectLayer<Tracer>(RenderLayer::Level4);

            auto o1 = buildCircle({0, 0});
            auto o2 = buildCircle({0, -2});
            auto o3 = buildCircle({0, -4});
            o3->m_mass = 0.003; 
            o2->m_mass = 0.003; 
            buildFixPoint(o1->m_position)
                ->target = o1;
            buildSpring(o1, o2, 2, 50, 0.1, 8);
            buildSpring(o2, o3, 2, 50, 0.1, 8);

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
            buildSpring(o1, o2, 2.5)->count = 8;

            auto circle = buildCircle(glm::vec2(1, 0), 0.3);
            circle->m_mass = 1;

            circle = buildCircle(glm::vec2(0, 0), 0.4);
            circle->m_mass = 0.5;
            auto circle1 = buildCircle(glm::vec2(2, 0), 0.4);
            circle1->m_mass = 0.5;
            buildSpring(circle, circle1, 2, 90, 2, 8);

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

    Line line = Line({5, -3}, {-5, -3}, 0.2);
    Line b1 = Line({1.4, -3}, {3, -2.7}, 0.4);
    Line b2 = Line({-1.4, -3}, {-3, -2.7}, 0.4);
    void update(const real& dt) {
        auto& objects = world.getObjects<Circle>();
        for (auto& obj : objects) {
            auto circle = static_cast<Circle*>(obj);
            if (line.collide(circle)) {
                line.resolve_collision(circle);
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

        renderer.renderLine(proj, line.p[0], line.p[1], glm::vec3(1), 0.01);
        renderer.renderLine(proj, b1.p[0], b1.p[1], glm::vec3(1), 0.01);
        renderer.renderLine(proj, b2.p[0], b2.p[1], glm::vec3(1), 0.01);
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


Simulation* Simulation::Instance = new DemoSimulation();

mfw::Application* mfw::CreateApplication() {

    PhysicsEmulator* emulator = new PhysicsEmulator();
    emulator->world_scale = 6 * Simulation::Get()->getWorldScale();
    emulator->shift_rate = 0.001 * emulator->world_scale;
    emulator->zoom_rate = 0.01 * emulator->world_scale;
    emulator->settings.sub_step = 500;

    return emulator;
}

