#include "Simulation.h"

#include "DistanceConstraint.h"
#include "PointConstraint.h"
#include "Renderer.h"
#include "Circle.h"
#include "mfwlog.h"
#include "Input.h"
#include "Application.h"
#include <list>

void Simulation::update(const f64& dt) {
    world.update(dt);
}
void Simulation::render(mfw::Renderer& renderer) {
    world.render(camera.getProjection(), renderer);
}

void Simulation::addString(const glm::vec2& pos, u32 node, f32 length) {
    ASSERT(node > 1);

    auto p1 = world.addObject<Circle>(glm::vec2(0, 0) + pos, attri.node_color, attri.node_size);
    for (f32 i = 1; i < node; i++) {
        auto p2 = world.addObject<Circle>(glm::vec2(0, -i * length) + pos, attri.node_color, attri.node_size);
        world.addConstraint<DistanceConstraint>(p1, p2, length, attri.line_width);
        p1 = p2;
    }
}

void Simulation::addCircle(const glm::vec2& pos, i32 n, f32 r, i32 nstep) {
    ASSERT(r > 0 && n > 2 && nstep > 0);
    static const f32 pi = 3.14159265359f;

    f32 ri = 2 * pi / n;
    f32 nlen = glm::length(r * glm::vec2(sin(0) - sin(ri), cos(0) - cos(ri)));

    auto center = world.addObject<Circle>(glm::vec2(0) + pos, attri.node_color, attri.node_size);
    center->drawEnable = false;

    Object* p1 = world.addObject<Circle>(r * glm::vec2(sin(0), cos(0)) + pos, attri.node_color, attri.node_size);
    Object* first = p1;
    p1->drawEnable = false;
    Object* p2;

    for (i32 i = 1; i < n; i++) {
        f32 a = ri * i;
        p2 = world.addObject<Circle>(r * glm::vec2(sin(a), cos(a)) + pos, attri.node_color, attri.node_size);
        p2->drawEnable = false;
        world.addConstraint<DistanceConstraint>(p1, center, r, attri.line_width);
        world.addConstraint<DistanceConstraint>(p1, p2, nlen, attri.line_width);
        p1 = p2;
    }

    world.addConstraint<DistanceConstraint>(p1, first, nlen, attri.line_width);
    world.addConstraint<DistanceConstraint>(p1, center, r, attri.line_width);
}

void Simulation::addBox(const glm::vec2& pos, f32 l)
{
    ASSERT(l > 0);
    addCircle(pos, 4, sqrt(2.0f) * l * 0.5);
}

void Simulation::addTriangle(const glm::vec2& pos, f32 l) {
    ASSERT(l > 0);
    addCircle(pos, 3, l);
}

void Simulation::addDoublePendulum(f64 angle, f64 d) {
    f64 r = angle * 3.14 / 180;
    glm::dvec2 direction = glm::normalize(glm::dvec2(cos(r), sin(r))) * d * (f64)unitScale;
    auto p1 = world.addObject<Circle>(glm::vec2(), attri.node_color, attri.node_size);
    auto p2 = world.addObject<Circle>(direction, attri.node_color, attri.node_size);
    auto p3 = world.addObject<Circle>(direction * 2.0, attri.node_color, attri.node_size);
    world.addConstraint<DistanceConstraint>(p1, p2, d * unitScale, attri.line_width);
    world.addConstraint<DistanceConstraint>(p2, p3, d * unitScale, attri.line_width);
    addFixPointConstraint(glm::vec2(), attri.node_size * 1.5)
        ->target = p1;
    p3->m_mass = 0.3f;
    p3->r = p3->m_mass * unitScale;
}

void Simulation::SetupRotateBox() {
    addBox(glm::vec2(), 3 * unitScale);

    auto& circles = world.getObjects<Circle>();
    i32 len = circles.size();
    auto boxCenter = circles[len - 5];
    auto c1 = circles[len - 2];
    auto c2 = circles[len - 4];
    addFixPointConstraint(glm::vec2(), attri.node_size * 1.5)
        ->target = boxCenter;

    auto h1 = addHorizontalPointConstraint(glm::vec2(6, 0) * unitScale, attri.node_size * 1.5);
    auto h2 = addHorizontalPointConstraint(glm::vec2(-6, 0) * unitScale, attri.node_size * 1.5);
    auto p1 = world.addObject<Circle>(h1->self.m_pos, attri.node_color, attri.node_size);
    auto p2 = world.addObject<Circle>(h2->self.m_pos, attri.node_color, attri.node_size);
    f64 l = glm::length(p1->m_pos - c1->m_pos);
    world.addConstraint<DistanceConstraint>(c1, p1, l, attri.line_width);
    world.addConstraint<DistanceConstraint>(c2, p2, l, attri.line_width);
    h1->target = p1;
    h2->target = p2;
}

PointConstraint* Simulation::addHorizontalPointConstraint(const glm::dvec2& pos, f32 r) {
    auto result = world.addConstraint<PointConstraint>(r, [](const f64& dt, PointConstraint* pc) {
                    (void) dt;
                    if (pc->target) {
                        pc->self.m_pos.x = pc->target->m_pos.x;
                        pc->target->m_pos.y = pc->self.m_pos.y;
                        pc->target->m_acceleration.y = 0;
                        pc->target->m_velocity.y = 0;
                    }
                });
    result->onRender = [=](const glm::mat4& proj, mfw::Renderer& renderer, PointConstraint* pc) {
        Circle(pc->self.m_pos, pc->self.m_color, pc->d).draw(proj, renderer);
    };

    result->self = Object(pos, 0, glm::vec3(COLOR(0x3c4467)));
    return result;
}

PointConstraint* Simulation::addFixPointConstraint(const glm::dvec2& pos, f32 r) {
    auto result = world.addConstraint<PointConstraint>(r, [](const f64& dt, PointConstraint* pc) {
                    (void) dt;
                    if (pc->target) {
                        pc->target->m_pos = pc->self.m_pos;
                        pc->target->m_acceleration = {};
                        pc->target->m_velocity = {};
                    }
                });
    result->onRender = [=](const glm::mat4& proj, mfw::Renderer& renderer, PointConstraint* pc) {
        Circle(pc->self.m_pos, pc->self.m_color, pc->d).draw(proj, renderer);
    };

    result->self = Object(pos, 0, glm::vec3(COLOR(0x486577)));
    return result;
}

void Simulation::addTracer(World& world, Object* target, i32 samples) {
    auto result = world.addConstraint<PointConstraint>(0, [](const f64& dt, PointConstraint* pc) {
                    (void) dt; (void)pc;
                });

    result->target = target;
    result->onRender = [=](const glm::mat4& proj, mfw::Renderer& renderer, PointConstraint* pc) {
        static const i32 max = samples;
        static const f32 maxScale = 0.1 * unitScale,
                         minScale = 0.015 * unitScale,
                         dr = 0.6f;
        static std::list<glm::vec2> positions_trace;

        if ((i32)positions_trace.size() == max) {
            positions_trace.pop_front();
        }
        positions_trace.push_back(pc->target->m_pos);

        f32 i = 0;
        for (auto iter = positions_trace.begin();;) {
            const glm::vec2 p1 = *(iter++);
            if (iter == positions_trace.end())
                break;
            const glm::vec2 p2 = *iter;
            const glm::vec3 trace = glm::vec3(COLOR(0xc73e3e)), background = glm::vec3(COLOR(0x191919));
            const glm::vec3 color = (trace - background) * (i++ / positions_trace.size()) + background;
            f32 t = maxScale * (i / positions_trace.size());
            t = glm::clamp(t - maxScale * dr, minScale, maxScale);
            renderer.renderCircleI(proj, { p2, color, t });
            renderer.renderCircleI(proj, { p1, color, t });
            renderer.renderLineI(proj, p1, p2, color, t);
        }
    };
}

glm::dvec2 Simulation::mouseToWorldCoord() {
    mfw::Window* main = mfw::Application::Get()->GetWindow();
    auto& mouse = mfw::Input::GetMouse();
    f32 width = main->width(), height = main->height();
    glm::vec4 uv = glm::vec4(mouse.first / width, 1 - mouse.second / height, 0, 0) * 2.0f - 1.0f;
    return camera.view * (uv * ((glm::vec4(1) / camera.ortho) / camera.scale));
}

